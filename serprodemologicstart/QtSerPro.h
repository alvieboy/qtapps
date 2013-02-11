#include <QObject>
#include <SerPro/SerPro.h>
#include <SerPro/SerProHDLC.h>
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>
#include <QTimer>

#ifdef QT_USE_NAMESPACE_SERIALPORT
QT_USE_NAMESPACE_SERIALPORT
#endif

class SerialWrapper
{
public:
	static void write(uint8_t v);
	static void write(const unsigned char *buf, unsigned int size);
	static void flush();
	static bool waitEvents(bool block);
};

class TimerEntry: public QTimer
{
public:
	Q_OBJECT;
public:
	TimerEntry(int (*cb)(void*), void *data): QTimer(), cb(cb), data(data)
	{
	}
public slots:
	void call();
protected:
	int (*cb)(void*);
	void *data;
};

class SerProQTTimer: public QTimer
{
public:
	Q_OBJECT
public:
	typedef TimerEntry *timer_t;

	static timer_t addTimer( int (*cb)(void*), int milisseconds, void *data=0);
	static timer_t cancelTimer(const timer_t &t);
	static inline bool defined(const timer_t &t){
		return t != 0;
	}
};

struct HDLCConfig: public HDLC_DefaultConfig
{
	static unsigned int const stationId = 0xFF; /* Only for HDLC */
};

struct HDLCConfigMaster: public HDLC_DefaultConfig
{
	static unsigned int const stationId = 0xFF; /* Only for HDLC */
    static HDLCImplementationType const implementationType = FULL;
};

struct SerProConfig {
	static unsigned int const maxFunctions = 255;
	static unsigned int const maxPacketSize = 8192;
	static SerProImplementationType const implementationType = Master;
	typedef HDLCConfigMaster HDLC;
};

struct SerProConfigSlave {
	static unsigned int const maxFunctions = 255;
	static unsigned int const maxPacketSize = 1024;
	static SerProImplementationType const implementationType = Slave;
	typedef HDLCConfig HDLC;
};

#define SERPRO_QT_BEGIN() \
   DECLARE_SERPRO_WITH_TIMER( SerProConfig, SerialWrapper, SerProHDLC, SerProQTTimer, SerPro)

#define SERPRO_QT_SLAVE_BEGIN() \
   DECLARE_SERPRO( SerProConfigSlave, SerialWrapper, SerProHDLC, SerPro)

#define SERPRO_QT_END() \
	SERPRO_EVENT(LINK_UP) { QtSerPro::connectEvent(); } \
	IMPLEMENT_SERPRO(255,SerPro,SerProHDLC); \
	void QtSerPro::start() { SerPro::connect(); } \
	void QtSerPro::initLayer() { SerPro::initLayer(); } \
	void QtSerPro::process(unsigned char c) { SerPro::processData(c); }

typedef int speed_t;


class QtSerPro: public QObject
{
	Q_OBJECT
public:
	friend class SerialWrapper;
private:
	QtSerPro(){}
public:
	static QtSerPro *create() {
		QtSerPro *inst = new QtSerPro();
		us=inst;
		return us;
	}
	typedef enum { CONNECTED, DISCONNECTED } connection_state_t;

	void connect();


Q_SIGNALS:

	void onConnectionStateChanged(bool connected);

public slots:
	void readyReadDeferred();
	void dataReady();
public:
	QSerialPort *getPort() { return port; }
	static QtSerPro *getInstance() { return us; }

protected:
	/* Dont change these funcations */

	static void start();
	static void initLayer();
	static void process(unsigned char c);
public:
	static void connectEvent();
	void open(const QString &name, int baudrate);
private:

	QSerialPort *port;
	bool in_request, delay_request;
	bool connected;
	static QtSerPro*us;
};
