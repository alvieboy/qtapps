#include "portselectdialog.h"
#include "ui_portselectdialog.h"
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>

#ifdef QT_USE_NAMESPACE_SERIALPORT
QT_USE_NAMESPACE_SERIALPORT
#endif

PortSelectDialog::PortSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortSelectDialog)
{
	ui->setupUi(this);
	ui->PortComboBox->clear();

	foreach (const QSerialPortInfo&info, QSerialPortInfo::availablePorts()) {
		ui->PortComboBox->addItem(info.portName());
	}

	ui->BaudComboBox->clear();
	ui->BaudComboBox->addItem("115200");
	ui->BaudComboBox->addItem("1000000");
}

void PortSelectDialog::accept()
{
	emit  setPortAndBaudrate(ui->PortComboBox->currentText(), ui->BaudComboBox->currentText());
	close();
}

PortSelectDialog::~PortSelectDialog()
{
    delete ui;
}
