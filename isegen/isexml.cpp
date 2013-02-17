#include "isexml.h"
#include <QDomDocument>
#include <QFile>
#include <QObject>
#include <QStringList>

ISEXML::ISEXML(): doc(NULL)
{
}

int ISEXML::openProject(const QString &name, QString &error)
{
	if (doc)
		delete(doc);

	doc = new QDomDocument();

	QFile file(name);

	if (!file.open(QIODevice::ReadOnly))
		return -1;

	if (!doc->setContent(&file)) {
		file.close();
		return -1;
	}

    version = getElement(doc->documentElement(),"version");

	if (version.isNull()) {
		qDebug("Invalid");
		error = tr("Looks like an invalid ISE file");
		return -1;
	}
	if (getSchemaVersion()!="2") {
		error = tr("Schema version") + getSchemaVersion() + tr("not supported");
        return -1;
	}
	properties = getElement(doc->documentElement(), "properties");

	files = getElement(doc->documentElement(), "files");

	return 0;
}

QString ISEXML::getISEVersion() const
{
	if (!doc)
		return NULL;
	return version.attribute("xil_pn:ise_version");
}

QString ISEXML::getSchemaVersion() const
{
	if (!doc)
		return NULL;
	return version.attribute("xil_pn:schema_version");
}


QDomElement ISEXML::getElement(const QDomElement &root, const QString&name)
{
	if (root.isNull())
		return QDomElement();

	QDomNodeList list = root.elementsByTagName(name);
	if (list.isEmpty())
		return QDomElement();

	return list.item(0).toElement();
}

QDomElement ISEXML::getElementWithAttribute(const QDomElement &root, const QString&name, const QString&attribute, const QString&value)
{
	unsigned int idx;

	if (root.isNull())
		return QDomElement();

	QDomNodeList list = root.elementsByTagName(name);
	if (list.isEmpty())
		return QDomElement();

	for(idx=0; idx<list.length(); idx++) {
		QDomElement e = list.item(idx).toElement();
		if (e.attribute(attribute)==value)
			return e;
	}
    return QDomElement();
}

QString ISEXML::fileTypeToString(enum ISEFileType type)
{
	switch(type) {
	case FILE_VHDL:
		return "FILE_VHDL";
	default:
		return "";
	}
}

QString ISEXML::getProperty(const QString &name) const
{

	for(QDomNode n = properties.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if (n.isElement()) {
			QDomElement el = n.toElement();
			QString sname = el.attribute("xil_pn:name");
			if (name==sname) {
				return el.attribute("xil_pn:value");
			}
		}
	}
    return QString();
}

QDomElement ISEXML::addFile(const QString &filename, enum ISEFileType type)
{
	QDomElement file = doc->createElement("file");
	file.setAttribute("xil_pn:name",filename);
	file.setAttribute("xil_pn:type",fileTypeToString(type));
	if (type==FILE_VHDL) {
		/* Add association */
		QDomElement assoc = doc->createElement("association");
		assoc.setAttribute("xil_pn:name","BehavioralSimulation");
		assoc.setAttribute("xil_pn:seqID", getSequenceID());
		file.appendChild(assoc);

		assoc = doc->createElement("association");
		assoc.setAttribute("xil_pn:name","Implementation");
		assoc.setAttribute("xil_pn:seqID", getSequenceID());
		file.appendChild(assoc);

		incrementSequenceID();
	}
	return file;
}


int ISEXML::openConfiguration(const QString &config)
{
	QFile file(config);

	if (!file.open(QIODevice::ReadOnly))
		return -1;

	if (!m_configDoc.setContent(&file)) {
		file.close();
		return -1;
	}
	return 0;
}

QDomElement ISEXML::getConfigNode() const {
    return getElement(m_configDoc.documentElement(), "config");
}

QStringList ISEXML::attributesToList( QDomNodeList &e, const QString &attr)
{
	unsigned int idx;
	QStringList list;

	for(idx=0; idx<e.length(); idx++)
	{
		QDomNode n = e.at(idx);
		if (n.isElement()) {
			QDomElement el = n.toElement();
			QString sname = el.attribute(attr);
			list.push_back(sname);
		}
	}
    return list;
}

QList<QString> ISEXML::getPlatforms() const
{
	QList<QString> list;
	unsigned int idx;

	QDomNodeList platforms;

	QDomElement platformnode = getElement(getConfigNode(),"platforms");
	platforms = platformnode.elementsByTagName("platform");

	for(idx=0; idx<platforms.length(); idx++)
	{
		QDomNode n = platforms.at(idx);
		if (n.isElement()) {
			QDomElement el = n.toElement();
			QString sname = el.attribute("name");
			list.push_back(sname);
		}
	}
	return list;
}

QDomElement ISEXML::locatePlatform(const QString &name) const
{
	QDomElement platformnode = getElement(getConfigNode(),"platforms");
	QDomElement e = getElementWithAttribute(platformnode,"platform", "name", name);
	return e;
}

QDomElement ISEXML::locateBoard(const QString&platform, const QString &name) const
{
	QDomElement e = locatePlatform(platform);
	if (e.isNull())
		return e;

	e = getElement(e,"boards");
	e = getElementWithAttribute(e,"board", "name", name);
	
	return e;
}

QDomElement ISEXML::locateVariant(const QString&platform, const QString &board, const QString&name) const
{
	QDomElement e = locateBoard(platform,board);
	if (e.isNull())
		return e;

	e = getElement(e,"variants");
	e = getElementWithAttribute(e,"variant", "name", name);
	
	return e;
}

QList<QString> ISEXML::getBoards(const QString &platform) const
{
	QDomElement e = locatePlatform(platform);
	if (e.isNull())
		return QList<QString>();

	QDomElement boardsnode = getElement(e,"boards");

	QDomNodeList boards = boardsnode.elementsByTagName("board");
	QStringList list = attributesToList(boards,"name");
    return list;
}

QList<QString> ISEXML::getVariants(const QString &platform, const QString &board) const
{
    QDomElement e = locatePlatform(platform);
	if (e.isNull())
		return QList<QString>();

	QDomElement boardsnode = getElement(e,"boards");
    e = getElementWithAttribute(boardsnode,"board", "name", board);

	if (e.isNull())
		return QList<QString>();

	QDomNodeList variants = e.elementsByTagName("variant");
	QStringList list = attributesToList(variants,"name");
    return list;
}

PlatformInformation ISEXML::getPlatformInformation(const QString &platform)
{
	PlatformInformation i;
	QDomElement e = locatePlatform(platform);

	if (e.isNull())
		return i;

	e = getElement(e,"description");

	if (e.isNull())
		return i;

	i.description = e.text();
	return i;
}

BoardInformation ISEXML::getBoardInformation(const QString &platform, const QString &board)
{
	BoardInformation i;
	QDomElement e = locateBoard(platform,board);

	if (e.isNull())
		return i;

	e = getElement(e,"description");

	if (e.isNull())
		return i;

	i.description = e.text();

	return i;
}

VariantInformation ISEXML::getVariantInformation(const QString &platform, const QString &board, const QString &variant)
{
	VariantInformation i;
	QDomElement e = locateVariant(platform,board,variant);

	if (e.isNull())
		return i;

	e = getElement(e,"description");

	if (e.isNull())
		return i;

	i.description = e.text();

	return i;
}
