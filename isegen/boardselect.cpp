#include "boardselect.h"
#include "ui_boardselect.h"
#include <QDebug>

BoardSelect::BoardSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BoardSelect)
{
    ui->setupUi(this);
}

BoardSelect::~BoardSelect()
{
    delete ui;
}

void BoardSelect::setXml(ISEXML *xmli)
{
	xml=xmli;
	QList<QString> projects = xml->getPlatforms();
	ui->projectComboBox->clear();
	ui->projectComboBox->addItems(projects);
}

void BoardSelect::onProjectChanged(QString project)
{
	PlatformInformation i;

	ui->boardComboBox->clear();
	ui->variantComboBox->clear();

	QList<QString> boards = xml->getBoards(project);

	ui->boardComboBox->addItems(boards);

	i = xml->getPlatformInformation(project);
	ui->labelProject->setText(i.description);
}

void BoardSelect::onVariantChanged(QString variant)
{
	VariantInformation i;
	i = xml->getVariantInformation(ui->projectComboBox->currentText(), ui->boardComboBox->currentText(),variant);
	ui->labelVariant->setText(i.description);
}

void BoardSelect::onBoardChanged(QString board)
{
	BoardInformation i;

	ui->variantComboBox->clear();
	//	qDebug()<<" Looking up "<<board<< " in "<<ui->projectComboBox->currentText();
	QList<QString> variants = xml->getVariants(ui->projectComboBox->currentText(), board);
	ui->variantComboBox->addItems(variants);

	i = xml->getBoardInformation(ui->projectComboBox->currentText(), board);
	ui->labelBoard->setText(i.description);
}
