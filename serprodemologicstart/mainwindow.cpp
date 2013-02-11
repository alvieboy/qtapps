#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "portselectdialog.h"
#include "QtSerPro.h"
#include <QKeyEvent>

SERPRO_QT_BEGIN();

IMPORT_FUNCTION(1, toggleLed, void(uint8_t));
IMPORT_FUNCTION(2, setLed, void(uint8_t,uint8_t));
IMPORT_FUNCTION(3, lcdSetBrightness, void(uint8_t));
IMPORT_FUNCTION(4, setValues, void (uint32_t values, uint8_t mask));

SERPRO_QT_END();

#define NONE 0
#define SA 1
#define SB 2
#define SC 4
#define SD 8
#define SE 16
#define SF 32
#define SG 64

#define ANENTRY(a,b,c,d) ((a)+((b)<<8)+((c)<<16)+((d)<<24))

#define SEG_Z SA|SB|SG|SE|SD
#define SEG_P SA|SB|SF|SG|SE
#define SEG_U SB|SF|SE|SC|SD
#define SEG_i SC
#define SEG_n SE|SG|SC
#define SEG_o SE|SG|SC|SD

const uint32_t animation[] =
{
	ANENTRY( NONE       , NONE         , NONE,         NONE ),
	ANENTRY( NONE       , NONE         , NONE,         SEG_Z ),
	ANENTRY( NONE       , NONE         , SEG_Z,        SEG_P ),
	ANENTRY( NONE       , SEG_Z        , SEG_P,        SEG_U ),
	ANENTRY( SEG_Z      , SEG_P        , SEG_U,        SEG_i ),
	ANENTRY( SEG_P      , SEG_U        , SEG_i,        SEG_n ),
	ANENTRY( SEG_U      , SEG_i        , SEG_n,        SEG_o ),
	ANENTRY( SEG_i      , SEG_n        , SEG_o,        NONE ),
	ANENTRY( SEG_n      , SEG_o        , NONE,         NONE ),
	ANENTRY( SEG_o      , NONE         , NONE,         NONE ),
};

#define ANIMATIONCOUNT (sizeof(animation)/sizeof(uint32_t) )

#define ANIMATION_SPEED 125

#ifdef QT_USE_NAMESPACE_SERIALPORT
QT_USE_NAMESPACE_SERIALPORT
#endif

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	sp = QtSerPro::create();
	ui->setupUi(this);
	bConnected=false;
	animateTimer = new QTimer;

	connect( sp, SIGNAL(onConnectionStateChanged(bool)), this, SLOT(onConnect(bool)));
}

void MainWindow::animateTick()
{
	if (animstep>=ANIMATIONCOUNT)
		animstep = 0;

	setValues(animation[animstep], 0xf); // All up mask

	animstep++;
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
	int led=-1;
	if (bConnected) {

		if (event->type()==QEvent::KeyPress) {
			if (event->key()==(Qt::Key_1)) led=0;
			if (event->key()==(Qt::Key_2)) led=1;
			if (event->key()==(Qt::Key_3)) led=2;
			if (event->key()==(Qt::Key_4)) led=3;
			if (event->key()==(Qt::Key_5)) led=4;
			if (event->key()==(Qt::Key_6)) led=5;
			if (event->key()==(Qt::Key_7)) led=6;
			if (event->key()==(Qt::Key_8)) led=7;
			if (led>=0)
				toggleLed(led);
		}
	}
	QMainWindow::keyPressEvent(event);
}

void MainWindow::onConnect(bool connected)
{
	int i;

	if (!connected) {
		if (animateTimer)
			animateTimer->stop();
		bConnected=false;
		return;
	}

	/* Clear all LEDs */

	for (i=0;i<8; i++) {
		setLed(i,0);
	}

	/* Update slider */
	ui->brightnessSlider->setValue(8);
	bConnected=true;

	ui->textEdit->append("Connected! Use keys 1 to 8 to toggle the leds.");

	if (animateTimer) {
		animstep = 0;
		connect(animateTimer, SIGNAL(timeout()), this, SLOT(animateTick()));
		animateTimer->start(ANIMATION_SPEED);
	}
}

void MainWindow::onBrightnessChanged(int value)
{
	if (bConnected)
		lcdSetBrightness(value);
}

MainWindow::~MainWindow()
{
	if (animateTimer)
		delete animateTimer;

	delete ui;
	delete sp;
	QCoreApplication::exit(0);
}


void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::onSetupAction()
{
	PortSelectDialog menu(this);
	QObject::connect(&menu,
					 SIGNAL(setPortAndBaudrate(QString,QString)),
					 this,
					 SLOT(onSetPortAndBaudrate(QString,QString))
		   );
	menu.exec();
}

void MainWindow::onSetPortAndBaudrate(const QString &port,const QString &baud)
{
	QString s = "Selected port "+port+" at "+baud+" baud";

	ui->statusBar->showMessage(s);
	baudrate = baud.toInt();
	portname = port;
}

void MainWindow::doConnect()
{
	if (!portname.length()) {
		ui->statusBar->showMessage("No port selected. Use menu to select a port.");
        return;
	}
	if (bConnected) {
        return;
	}
	QtSerPro::getInstance()->open( portname, baudrate );
	ui->textEdit->append(QString("Attempting to connect to ")+portname);
}
