#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	// connect to websocket
	//TODO: nupp connect // connected
	connect(&m_webSocket, &QWebSocket::connected, this, &MainWindow::onConnected);
	connect(&m_webSocket, &QWebSocket::disconnected, this, &MainWindow::onDisconnected);
	//connect(&m_webSocket,  &QWebSocket::error, this, &MainWindow::onError);
	//	 this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
		  [=](QAbstractSocket::SocketError error){
		qDebug() <<error << m_webSocket.errorString();
	});


	// try to open socket on start
	open();
}

MainWindow::~MainWindow()
{
	m_webSocket.close();
	delete ui;
}

void MainWindow::open()
{
	QString url =  QString("ws://%1:%2").arg(ui->wsLineEdit->text()).arg(ui->wsPortSpinBox->value());
	m_webSocket.open(QUrl(url));
	//TODO: handle error
	qDebug() << m_webSocket.errorString();

}


void MainWindow::onConnected()
{
	connect(&m_webSocket, &QWebSocket::textMessageReceived,
			this, &MainWindow::onTextMessageReceived);
	ui->wsConnectButton->setText("Connected");
	ui->wsConnectButton->setEnabled(false);
	m_webSocket.sendTextMessage("ws2osc");
}

void MainWindow::onDisconnected()
{
	qDebug() << "Socket disconnected";
	ui->wsConnectButton->setText("Connect");
	ui->wsConnectButton->setEnabled(true);
}

void MainWindow::onError(QAbstractSocket::SocketError error)
{
	qDebug() << error;
}

void MainWindow::onTextMessageReceived(QString message)
{
	qDebug() << "Message received:" << message;
	ui->messagesTextEdit->appendPlainText(message);

}

void MainWindow::on_wsConnectButton_clicked()
{
	open();
}
