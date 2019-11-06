#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>
#include "wsserver.h"

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
	void setClientsCount(int clientsCount);

private slots:



	void on_clearButton_clicked();

	void on_analyzeButton_clicked();

private:
	Ui::MainWindow *ui;
	WsServer *wsServer;

};

#endif // MainWindow_H
