#include "isexml.h"
#include <QDomDocument>
#include <QFile>
#include <QObject>
#include <QStringList>
#include <QDebug>

ISEXML::ISEXML(): doc(NULL)
{
}

void ISEXML::createBare()
{
	if (doc)
		delete doc;

	doc = new QDomDocument();
	QDomProcessingInstruction xmlDecl = doc->createProcessingInstruction("xml",
																		 "version=\"1.0\" "
																		 "encoding=\"UTF-8\" "
																		 "standalone=\"no\"");
	doc->appendChild(xmlDecl);

	QDomElement project = doc->createElement("project");
	project.setAttribute("xmlns", "http://www.xilinx.com/XMLSchema");
	project.setAttribute("xmlns:xil_pn","http://www.xilinx.com/XMLSchema");

	doc->appendChild(project);

	QDomElement header = doc->createElement("header");
	project.appendChild(header);

	QDomElement version = doc->createElement("version");
	version.setAttribute("xil_pn:ise_version","14.1");
	version.setAttribute("xil_pn:schema_version","2");
	project.appendChild(version);

	project.appendChild(doc->createElement("files"));
	project.appendChild(doc->createElement("properties"));
	project.appendChild(doc->createElement("bindings"));
	project.appendChild(doc->createElement("libraries"));
}

QDomElement ISEXML::getVersion()
{
	if (NULL==doc)
        return QDomElement();
	return getElement(doc->documentElement(),"version");
}

QDomElement ISEXML::getProperties()
{
	if (NULL==doc)
        return QDomElement();
	return getElement(doc->documentElement(),"properties");
}

QDomElement ISEXML::getFiles()
{
	if (NULL==doc)
        return QDomElement();
	return getElement(doc->documentElement(),"files");
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

	if (getVersion().isNull()) {
		qDebug("Invalid");
		error = tr("Looks like an invalid ISE file");
		return -1;
	}

	if (getSchemaVersion()!="2") {
		error = tr("Schema version") + getSchemaVersion() + tr("not supported");
        return -1;
	}

	return 0;
}

QString ISEXML::getISEVersion() 
{
	if (!doc)
		return NULL;
	return getVersion().attribute("xil_pn:ise_version");
}

QString ISEXML::getSchemaVersion()
{
	if (!doc)
		return NULL;
	return getVersion().attribute("xil_pn:schema_version");
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
	case FILE_UCF:
		return "FILE_UCF";
	case FILE_SCH:
        return "FILE_SCHEMATIC";
	default:
		return "";
	}
}

QString ISEXML::getProperty(const QString &name)
{

	for(QDomNode n = getProperties().firstChild(); !n.isNull(); n = n.nextSibling())
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
	if (type==FILE_VHDL || type==FILE_SCHEMATIC) {
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
	} else if (type==FILE_UCF) {
		QDomElement assoc = doc->createElement("association");
		assoc.setAttribute("xil_pn:name","Implementation");
		assoc.setAttribute("xil_pn:seqID", "0");
		file.appendChild(assoc);
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

void ISEXML::addFilesFromElement(QDomElement files)
{
	unsigned int i;

	if (!files.isNull()) {
		QDomNodeList fileslist = files.elementsByTagName("file");
		unsigned count = fileslist.count();

		qDebug() << count << "files";

		for (i=0; i < count; i++) {
			QDomElement e = fileslist.at(i).toElement();
			if (e.isNull())
                continue;
			QString type = e.attribute("type");
			ISEFileType t;

			if (type=="FILE_VHDL") {
                t = FILE_VHDL;
			} else if (type=="FILE_UCF") {
                t = FILE_UCF;
			} else {
				// Error?
                qDebug() << "Invalid file type "<<type;
                return;
			}
			QDomElement fe = addFile(e.text(), t);

			getFiles().appendChild(fe);
		}
	} else {
        qDebug() << "No files to add ????????";
	}
}

void ISEXML::addPropertiesFromElement(QDomElement props)
{
	unsigned int i;

	if (!props.isNull()) {
		QDomNodeList list = props.elementsByTagName("property");
		unsigned count = list.count();

		for (i=0; i < count; i++) {
			QDomElement e = list.at(i).toElement();
			if (e.isNull())
                continue;
			QString name = e.attribute("name");
            QString value = e.attribute("value");
			QDomElement fe = doc->createElement("property");
			fe.setAttribute("xil_pn:name", name);
			fe.setAttribute("xil_pn:value", value);
			fe.setAttribute("xil_pn:valueState","non-default");

			getProperties().appendChild(fe);
		}
	} else {
        qDebug() << "No files to add ????????";
	}
}

void ISEXML::updateProject(const QString &platform, const QString &board, const QString &variant)
{
	QDomElement p = locatePlatform(platform);
	QDomElement b = locateBoard(platform,board);
	QDomElement v = locateVariant(platform,board,variant);

	if (p.isNull() || b.isNull() || v.isNull())
		return;

	/* Clear sequence */
    sequence=0;
	/* Ok, grab files */
	QDomElement files = getElement(b,"files");
	addFilesFromElement(files);
    addPropertiesFromElement( getElement(b,"properties"));

    files = getElement(v,"files");
	addFilesFromElement(files);
	addPropertiesFromElement( getElement(v,"properties"));

    qDebug() << "Finished";

//	qDebug() << "Document" <<doc->toString();

	QFile file("test.xise");

	if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
		return;

	file.write(doc->toByteArray());
    file.close();
}

