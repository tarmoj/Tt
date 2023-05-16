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

	void on_analyzeButton_clicked();

	void on_cardNumberSpinBox_valueChanged(int value);

	void on_setOscButton_clicked();

	void on_sendOscButton_clicked();

	void on_emulateButton_clicked();

    void on_pushButton_clicked();

    void on_manualEntryCheckBox_toggled(bool checked);

    void on_votersCountSpinBox_valueChanged(int arg1);

private:
	Ui::MainWindow *ui;
	WsServer *wsServer;

};

#endif // MainWindow_H
