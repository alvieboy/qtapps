#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "isexml.h"
#include <QSettings>

#define CONFIG_USER_CREATE_DIRECTORY "UserCreateDirectory"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void onExit();
    void onSave();
	void onOpen();
    void onNew();

protected:
	void openFile(const QString &name);
private:
	Ui::MainWindow *ui;
	QSettings settings;
	ISEXML xml;
};

#endif // MAINWINDOW_H
