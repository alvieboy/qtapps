#ifndef BOARDSELECT_H
#define BOARDSELECT_H

#include <QDialog>
#include "isexml.h"

namespace Ui {
class BoardSelect;
}

class BoardSelect : public QDialog
{
    Q_OBJECT
    
public:
	explicit BoardSelect(QWidget *parent = 0);
	~BoardSelect();
	void setXml(ISEXML *xmli);

	QString getPlatform() const;
	QString getBoard() const;
	QString getVariant() const;

	void setDefaultDir(const QString&);
	QString getDefaultDir() const;

protected:
    void updateDirName();

public slots:
	void onProjectChanged(QString);
	void onProjectNameChanged(QString);
	void onVariantChanged(QString);
	void onBoardChanged(QString);
	void onDirectoryChoose();

private:
	Ui::BoardSelect *ui;
	ISEXML*xml;
	QString createDir;
};

#endif // BOARDSELECT_H
