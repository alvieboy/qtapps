#ifndef __ISEXML_H__
#define __ISEXML_H__


#include <QDomElement>
#include <QDomDocument>
#include <QObject>

struct BoardInformation
{
    QString description;
};

struct PlatformInformation
{
    QString description;
};

struct VariantInformation
{
    QString description;
};


class ISEXML: public QObject {
    Q_OBJECT;
public:

	ISEXML();

	int openConfiguration(const QString &config);

	int openProject(const QString &name, QString &error);
	void closeProject();

	enum ISEFileType { FILE_VHDL };


	QString fileTypeToString(enum ISEFileType type);
	QString getISEVersion() const;
	QString getSchemaVersion() const;
	QString getProperty(const QString &name) const;

	QList<QString> getPlatforms() const;
	QList<QString> getBoards(const QString &platform) const;
	QList<QString> getVariants(const QString &platform, const QString &board) const;

	PlatformInformation getPlatformInformation(const QString &platform);
	BoardInformation getBoardInformation(const QString &platform, const QString &board);
	VariantInformation getVariantInformation(const QString &platform, const QString &board, const QString &variant);

	QDomElement getConfigNode() const;

	QDomElement locatePlatform(const QString &name) const;
	QDomElement locateBoard(const QString&platform, const QString &name) const;
	QDomElement locateVariant(const QString&platform, const QString &board, const QString&name) const;

protected:
	QDomElement addFile(const QString &filename, enum ISEFileType type=FILE_VHDL);

	int getSequenceID() { return sequence; }
	void incrementSequenceID() { sequence++; }

    /* Helpers */
	static QDomElement getElement(const QDomElement &node, const QString&name);
	static QDomElement getElementWithAttribute(const QDomElement &root, const QString&name, const QString&attribute, const QString&value);
	static QStringList attributesToList( QDomNodeList &e, const QString &attr);



private:
	QDomDocument *doc;
	QDomElement version;
	QDomElement files;
	QDomElement properties;
	QDomDocument m_configDoc;
	int sequence;
};
#endif
