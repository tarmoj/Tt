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
    QVector<double>  test(4);
    test[0] = 0.1; test[1]= 0.2;
    resultsHash.insert("my", test);
    qDebug() << resultsHash.value("my")[1];
    QVector<double>  test2 = resultsHash.value("my");
    test2[1] = 0.5;
    resultsHash.insert("my", test2);
    qDebug() << resultsHash.value("my")[1];
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

	emit newMessage(message);
    //QString const keyword = "column";

    // message comes in column_voteNumber_columNumber_value_column // "card_<nr>_column_<nr>_<value>[_column_<nr>_<value>]"

    if (message.startsWith("card")) {
        QStringList messageParts = message.split("_");
        int cardNUmber = messageParts[2].toInt();
        while (messageParts.contains( "column")) {
            int dataIndex = messageParts.indexOf("column");
            if (messageParts.size() >= dataIndex + 2) {
                int column = messageParts[dataIndex+1].toInt();
                double value = messageParts[dataIndex+2].toFloat();
                qDebug() << "data at " << dataIndex << " column: "  << column << " value: " << value;
            }
            messageParts.removeAt(dataIndex);

        }


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


