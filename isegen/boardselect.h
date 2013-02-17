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
public slots:
	void onProjectChanged(QString);
	void onVariantChanged(QString);
	void onBoardChanged(QString);

private:
	Ui::BoardSelect *ui;
	ISEXML*xml;
};

#endif // BOARDSELECT_H
