#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>
class QtSerPro;

namespace Ui {
    class MainWindow;
}
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
	void doConnect();
	void onSetupAction();
	void onConnect(bool);
	void onSetPortAndBaudrate(const QString &port,const QString &baud);
	void onBrightnessChanged(int);
	void animateTick();

protected:
	void changeEvent(QEvent *e);
	void updateSerialPorts();
	void keyPressEvent(QKeyEvent *event);
	QTimer *animateTimer;
private:
	bool bConnected;
	QString portname;
	int baudrate;
	Ui::MainWindow *ui;
	QtSerPro *sp;
	unsigned int animstep;
};

#endif // MAINWINDOW_H
