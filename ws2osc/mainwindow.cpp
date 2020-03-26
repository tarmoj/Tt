#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_oscAddress(nullptr)
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
	// and set default osc address
	on_oscSetButton_clicked();
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

void MainWindow::setOscAddress(QString host, quint16 port)
{
	qDebug()<<"Setting OSC address to: "<<host<<port;
	if (host.contains("localhost")) {
		host = "127.0.0.1"; // somehow localhost does not do here...
	}
	if (m_oscAddress == nullptr) {
		m_oscAddress = new QOscClient(QHostAddress(host), port, this);
	} else {
		m_oscAddress->setAddress(QHostAddress(host), port);
	}
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
	message = message.trimmed();
	qDebug() << "Message received:" << message;

	ui->messagesTextEdit->appendPlainText(message);
	if (m_oscAddress && message.startsWith("/")) {
		QStringList messageParts = message.split(" ");
		if (messageParts.count()>=3) {
			QString path = messageParts.takeFirst();
			QString types = messageParts.takeFirst();
			QList <QVariant> data;
			if (types.length()==messageParts.count()) {
				for (int i=0; i<messageParts.count(); i++) {
					if (types.at(i)=='i') {
						data << messageParts[i].toInt();
					} else if (types.at(i)=='d') {
						data << messageParts[i].toDouble();
					} else if (types.at(i)=='f') {
						data << messageParts[i].toFloat();
					} else if (types.at(i)=='b') {
						data << static_cast<bool>(messageParts[i].toInt()); // not correct, probably
					} else if (types.at(i)=='s') {
						data << messageParts[i];
					} else {
						qDebug() << "Unkonwn type " << types.at(i);
					}


				}
			qDebug() << path << data;
			m_oscAddress->sendData(path, data);
			} else {
				qDebug() << "types and data number do not match";
			}
		}
	}

}

void MainWindow::on_wsConnectButton_clicked()
{
	open();
}

void MainWindow::on_oscSetButton_clicked()
{
	setOscAddress( ui->oscLineEdit->text(), static_cast<quint16>(ui->oscSpinBox->value() ) );
}
