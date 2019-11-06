#include "wsserver.h"
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>


QT_USE_NAMESPACE



WsServer::WsServer(quint16 port, QObject *parent) :
    QObject(parent),
	m_pWebSocketServer(new QWebSocketServer(QStringLiteral("DarkWarmServer"),
                                            QWebSocketServer::NonSecureMode, this)),
	m_clients(),
	csoundClient(nullptr),
	currentClient(0),
	paused(false)
{
    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        qDebug() << "WsServer listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &WsServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &WsServer::closed);
    }

}



WsServer::~WsServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}


void WsServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &WsServer::processTextMessage);

    connect(pSocket, &QWebSocket::disconnected, this, &WsServer::socketDisconnected);

    m_clients << pSocket;
    emit newConnection(m_clients.count());
}



void WsServer::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (!pClient) {
        return;
    }
	qDebug()<<message;
	QString peerAdress = pClient->peerAddress().toString();

	emit newMessage(message);

    // message comes in column_voteNumber_columNumber_value_column // "card_<nr>_column_<nr>_<value>[_column_<nr>_<value>]"

    if (message.startsWith("card")) {
        QStringList messageParts = message.split("_");
		int cardNumber = messageParts[2].toInt();
		if (cardNumber<0 || cardNumber>8) {
			qDebug() << "Wrong card: " << cardNumber;
			return;
		}

		QVector <double> tempData(4*9);
		if (resultsHash.contains(peerAdress) ) {
			tempData = resultsHash.value(peerAdress);
		} else {
			tempData.fill(-1);
		}
		while (messageParts.contains( "column")) {

			int dataIndex = messageParts.indexOf("column");
            if (messageParts.size() >= dataIndex + 2) {
                int column = messageParts[dataIndex+1].toInt();
				double value = messageParts[dataIndex+2].toDouble();
				//qDebug() << "data at " << dataIndex << " column: "  << column << " value: " << value;
				tempData[cardNumber*4 + column]=value;
            }
            messageParts.removeAt(dataIndex);
        }
		resultsHash[peerAdress] = tempData;
    }

}


void WsServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        m_clients.removeAll(pClient);
        emit newConnection(m_clients.count());
        pClient->deleteLater();
	}
}



void WsServer::sendMessage(QWebSocket *socket, QString message )
{
    if (socket == 0)
    {
        return;
    }
    socket->sendTextMessage(message);

}



void WsServer::send2all(QString message)
{
	foreach (QWebSocket *socket, m_clients) {
		socket->sendTextMessage(message);
	}
}


