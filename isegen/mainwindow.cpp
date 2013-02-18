#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include "boardselect.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->verticalLayoutWidget->setVisible(false);
	if (xml.openConfiguration("isegenconfig.xml")<0) {
		qDebug()<<"Cannot load configuration file";
	}
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::onOpen()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Open ISE Project File"),
												"",
												tr("ISE Project Files (*.xise)"));
	if (file.length()) {
		qDebug() << "Opening " << file;
		openFile(file);
	}
}

void MainWindow::openFile(const QString &name)
{
	QString error;
	if ( xml.openProject(name,error)< 0) {
		qDebug()<<"Error: "<<error;
	}
	qDebug() << "Project for ISE "<<xml.getISEVersion()<<", device "<<xml.getProperty("Device");
	ui->verticalLayoutWidget->setVisible(true);
//	ui->formLayout->clear();
	ui->formLayout->addRow(QString(tr("FPGA Device")), new QLabel(xml.getProperty("Device")));


}

void MainWindow::onSave()
{
}

void MainWindow::onNew()
{
	BoardSelect s;
	s.setXml( &xml );
	s.exec();
	xml.createBare();
	xml.updateProject( s.getPlatform(), s.getBoard(), s.getVariant() );
}

void MainWindow::onExit()
{
}


