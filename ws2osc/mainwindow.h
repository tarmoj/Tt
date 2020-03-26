#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebSockets/QWebSocket>

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

private Q_SLOTS:
	void onConnected();
	void onDisconnected();
	void onError(QAbstractSocket::SocketError error);
	void onTextMessageReceived(QString message);


	void on_wsConnectButton_clicked();

private:
	Ui::MainWindow *ui;
	QWebSocket m_webSocket;
	//QUrl m_url;


};

#endif // MAINWINDOW_H
