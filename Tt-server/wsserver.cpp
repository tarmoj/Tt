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
	paused(false),
	f(0),a(0), c(0), i(0)
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
		int cardNumber = messageParts[1].toInt();
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

VoteResults WsServer::getStatistics(int card, int column)
{
	VoteResults results;
	QHashIterator<QString, QVector<double>> i(resultsHash);
	int count = 0;
	double sum = 0;
	while (i.hasNext()) {
		i.next();
		double value = i.value()[card*4 + column];
		qDebug() << i.key() << " " << value;
		if (value>=0) {
			count++;
			sum += value;
		}

	}
	qDebug() << "Count: " << count << " sum: " << sum;
	results.count = count;
	results.sum = sum;
	results.average = sum/count;
	return results;
}

void WsServer::analyze(int card)
{

	VoteResults results;
	int t = resultsHash.size();  // total numbe of voeters
	if (t==0) {
		qDebug() << "No answers, nothing to analyze!";
		return;
	}
	if (card==0) {
		x[1] = getStatistics(0,0).average;
		x[2] = getStatistics(0, 1).average;
		x[3] = getStatistics(0, 2).average;
		x[4] = getStatistics(0, 3).average;
		f = x[1]; a= x[2]; c=x[3]; i=x[4];
	} else if (card==1) {

		results = getStatistics(1,0);
		x[5] = (results.sum + x[1]*(t -results.count )) / t;
		results = getStatistics(1,1);
		x[6] = (results.sum + x[2]*(t -results.count )) / t;
		results = getStatistics(1,2);
		x[7] = (results.sum + x[3]*(t -results.count )) / t;
		results = getStatistics(1,3);
		x[8] = (results.sum + x[4]*(t -results.count )) / t;
		f = x[5]; a= x[6]; c=x[7]; i=x[8];

	}
	// etc
	qDebug() << "f: " << f << "a: " << a << " c: " << c << " i: " << i;
	// TODO: calculateParameters(f,a,c,i);
}

void WsServer::calculateParameters(double f, double a, double c, double i)
{
	double y,z, e, d, w, v, o, p;
}


