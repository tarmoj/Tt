#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebSockets/QWebSocket>
#include <QSettings>
#include "qosc/qoscclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void open();
	void setOscAddress(QString host, quint16 port);

private Q_SLOTS:
	void onConnected();
	void onDisconnected();
	void onError(QAbstractSocket::SocketError error);
	void onTextMessageReceived(QString message);


	void on_wsConnectButton_clicked();

	void on_oscSetButton_clicked();

private:
	Ui::MainWindow *ui;
	QWebSocket m_webSocket;
	QOscClient * m_oscAddress;
	QSettings * settings;



};

#endif // MAINWINDOW_H
