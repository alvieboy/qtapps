#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "isexml.h"


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
	ISEXML xml;
};

#endif // MAINWINDOW_H
