#include "QtSerPro.h"
#include <QCoreApplication>
#include <QDebug>

void SerialWrapper::write(uint8_t v) {
	if (QtSerPro::getInstance()->getPort())
		QtSerPro::getInstance()->getPort()->putChar(v);
}

void SerialWrapper::write(const unsigned char *buf, unsigned int size) {
	if (QtSerPro::getInstance()->getPort())
		QtSerPro::getInstance()->getPort()->write((const char*)buf,size);
}

void SerialWrapper::flush() {
	if (QtSerPro::getInstance()->getPort())
		QtSerPro::getInstance()->getPort()->flush();
}

bool SerialWrapper::waitEvents(bool block) {
	QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
	return true;
}

void QtSerPro::connectEvent()
{
	emit getInstance()->onConnectionStateChanged(true);
}

void QtSerPro::dataReady()
{
	char buf[128];
	int r;
	r = port->read (buf,sizeof(buf));
	if (r>0) {
		int n=0;
		while (n<r) {
			process((unsigned char)buf[n]);
			n++;
		}
	}
}

void QtSerPro::readyReadDeferred()
{
	QTimer::singleShot(0,this,SLOT(dataReady()));
}

void QtSerPro::open(const QString &name, int baudrate)
{
	if (port)
		delete port;
	port = new QSerialPort(name);
	port->open(QIODevice::ReadWrite);

	port->setBaudRate(baudrate);
	port->setDataBits(QSerialPort::Data8);
	port->setFlowControl(QSerialPort::NoFlowControl);
	port->setParity(QSerialPort::NoParity);
	port->setStopBits(QSerialPort::OneStop);

	start();

	QObject::connect(port, SIGNAL(readyRead()), this,
					 SLOT(readyReadDeferred()));

}

void TimerEntry::call()
{
	cb(data);
}

SerProQTTimer::timer_t SerProQTTimer::addTimer( int (*cb)(void*), int milisseconds, void *data)
{
	TimerEntry *t = new TimerEntry(cb,data);
	connect(t, SIGNAL(timeout()), t, SLOT(call()));
	t->start(milisseconds);
	return t;
}

SerProQTTimer::timer_t SerProQTTimer::cancelTimer(const timer_t &t) {
	t->stop();
	delete t;
	return 0;
}


QtSerPro *QtSerPro::us=NULL;
