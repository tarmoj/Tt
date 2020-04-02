#include "wsserver.h"
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>


QT_USE_NAMESPACE



WsServer::WsServer(quint16 port, QObject *parent) :
    QObject(parent),
	currentCard(0),
	m_pWebSocketServer(new QWebSocketServer(QStringLiteral("TtServer"),
                                            QWebSocketServer::NonSecureMode, this)),
	m_clients(),
	csoundClient(nullptr),
	currentClient(0),
	paused(false),
	m_oscAddress(nullptr),
	ws2osc_client(nullptr)
	/*,
	f(0),a(0), c(0), i(0)*/
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

	if (message.startsWith("ws2osc")) { // register ws2osc utility
		ws2osc_client = pClient;
		qDebug() << "ws2osc client registered from " << ws2osc_client->peerAddress().toString();
	}

	emit newMessage(message);

	if (message=="performer") {
		performers.append(pClient);
		qDebug() << "Number of performers: " << performers.count();
		QString parametersString;
		parametersString.sprintf("f: %.2f a: %.2f c: %.2f i: %.2f y: %.2f z: %.2f d: %.2f e: %.2f w: %.2f v: %.2f o: %.2f p: %.2f",
								 f, a, c, i, y, z, d, e, w, v, o, p );
		pClient->sendTextMessage(parametersString);

	}

	if (message=="voter") {
		voters.append(pClient);
		qDebug() << "Number of voters: " << voters.count();
		pClient->sendTextMessage("card_"+QString::number(currentCard));
	}

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
		resultsHash[peerAdress] = tempData; // TUNDUB, et siin ta ikkagi ei kirjuta üle, kui uuesti samalt aadressilt tuleb
    }

}


void WsServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        m_clients.removeAll(pClient);
		if (performers.contains(pClient)) {
			performers.removeAll(pClient);
		}
		if (voters.contains(pClient)) {
			voters.removeAll(pClient);
		}

        emit newConnection(m_clients.count());
        pClient->deleteLater();
	}
}



void WsServer::sendMessage(QWebSocket *socket, QString message )
{
	if (socket == nullptr)
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

void WsServer::sendToPerformers(QString message)
{
	foreach (QWebSocket *socket, performers) {
		socket->sendTextMessage(message);
	}
}

void WsServer::sendToVoters(QString message)
{
	qDebug()<<Q_FUNC_INFO << message;
	foreach (QWebSocket *socket, voters) {
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
	results.average = (count>0) ? sum/count : 0;
	return results;
}

void WsServer::analyze(int card)
{
	QVector <double> x(37);
	x.fill(0);

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
	} else if (card==1) { // stupid code, should be written out as loop...
		results = getStatistics(1,0);
		x[5] = (results.sum + x[1]*(t -results.count )) / t;
		results = getStatistics(1,1);
		x[6] = (results.sum + x[2]*(t -results.count )) / t;
		results = getStatistics(1,2);
		x[7] = (results.sum + x[3]*(t -results.count )) / t;
		results = getStatistics(1,3);
		x[8] = (results.sum + x[4]*(t -results.count )) / t;
		f = x[5]; a= x[6]; c=x[7]; i=x[8];
	} else if (card==2) {
		results = getStatistics(card,0);
		x[9] = (results.sum + x[5]*(t -results.count )) / t;
		results = getStatistics(card,1);
		x[10] = (results.sum + x[6]*(t -results.count )) / t;
		results = getStatistics(card,2);
		x[11] = (results.sum + x[7]*(t -results.count )) / t;
		results = getStatistics(card,3);
		x[12] = (results.sum + x[8]*(t -results.count )) / t;
		f = x[9]; a= x[10]; c=x[11]; i=x[12];
	} else if (card==3) {
		results = getStatistics(card,0);
		x[13] = (results.sum + x[9]*(t -results.count )) / t;
		results = getStatistics(card,1);
		x[14] = (results.sum + x[10]*(t -results.count )) / t;
		results = getStatistics(card,2);
		x[15] = (results.sum + x[11]*(t -results.count )) / t;
		results = getStatistics(card,3);
		x[16] = (results.sum + x[12]*(t -results.count )) / t;
		f = x[13]; a= x[14]; c=x[15]; i=x[16];
	} else if (card==4) {
		results = getStatistics(card,0);
		x[17] = (results.sum + x[13]*(t -results.count )) / t;
		results = getStatistics(card,1);
		x[18] = (results.sum + x[14]*(t -results.count )) / t;
		results = getStatistics(card,2);
		x[19] = (results.sum + x[15]*(t -results.count )) / t;
		results = getStatistics(card,3);
		x[20] = (results.sum + x[16]*(t -results.count )) / t;
		f = x[17]; a= x[18]; c=x[19]; i=x[20];
	} else if (card==5) { // not certain, if this is correct... from here on. Is it averages?
		x[21] = getStatistics(card,0).average;
		x[22] = getStatistics(card,1).average;
		x[23] = getStatistics(card,2).average;
		x[24] = getStatistics(card,3).average;
		f = x[21]; a= x[22]; c=x[23]; i=x[24];
	} else if (card==6) {
		x[25] = getStatistics(card,0).average;
		x[26] = getStatistics(card,1).average;
		x[27] = getStatistics(card,2).average;
		x[28] = getStatistics(card,3).average;
		f = x[25]; a= x[26]; c=x[27]; i=x[28];
	} else if (card==7) {
		x[29] = getStatistics(card,0).average;
		x[30] = getStatistics(card,1).average;
		x[31] = getStatistics(card,2).average;
		x[32] = getStatistics(card,3).average;
		f = x[29]; a= x[30]; c=x[31]; i=x[32];
	} else if (card==8) {
		x[33] = getStatistics(card,0).average;
		x[34] = getStatistics(card,1).average;
		x[35] = getStatistics(card,2).average;
		x[36] = getStatistics(card,3).average;
		f = x[33]; a= x[34]; c=x[35]; i=x[36];
	}




	// etc
	qDebug() << "count: " << t  << "f: " << f << "a: " << a << " c: " << c << " i: " << i;
	calculateParameters();
}

QPair<double, double> WsServer::getIntersection(double d1_x1, double d1_y1, double d1_x2, double d1_y2,
  double d2_x1, double d2_y1, double d2_x2, double d2_y2)  // intersection point of two diagonals given in relative coordinates
{
  QPair <double, double> results;

  double deltay1 = d1_y2 - d1_y1;
  double deltax1 = d1_x2 - d1_x1;
  double deltay2 = d2_y2 - d2_y1;
  double deltax2 = d2_x2 - d2_x1;
  double k1 = deltay1 / deltax1; // tõus
  double k2 = deltay2 / deltax2;

  double y, x;
  // formula for y: y = k1 * (x-d1_x1) + d1_y1;
  // x - result or replacing y into  y = k2 * (x-d2_x1) + d2_y1 and resolving for x:
  x = ( d1_y1 - k1*d1_x1 + k2*d2_x1 - d2_y1)/(k2-k1);
  y = k1 * (x-d1_x1) + d1_y1;

  x = (x>1) ? 1 : ( (x<0) ? 0 : x  ); // check for limits
  y = (y>1) ? 1 : ( (y<0) ? 0 : y  );

  results.first = x; results.second = y;

  return results;
}

void WsServer::setOscAddress(QString host, quint16 port)
{
	qDebug()<<"Setting OSC address to: "<<host<<port;
	if (m_oscAddress == nullptr) {
		m_oscAddress = new QOscClient(QHostAddress(host), port, this);
	} else {
		m_oscAddress->setAddress(QHostAddress(host), port);
	}
}

void WsServer::sendMainParameters()
{
	QList <QVariant> data;
	data << f << a << c << i ;
	m_oscAddress->sendData("/Tt",  data  );
	osc2ws("/Tt", data);
}

void WsServer::emulate()
{
	f = rand() / double(RAND_MAX);
	a = rand() / double(RAND_MAX);
	c = rand() / double(RAND_MAX);
	i = rand() / double(RAND_MAX);
	calculateParameters();
}

void WsServer::calculateParameters()
{
	//double y,z, e, d, w, v, o, p;
	// TODO: on some occasions (if two values exactly the same, gives nan. Maybe some error somewhere... check that f!= c, a != 1
	if (int(f*100)==int(c*100)) {
		f += 0.01; // TODO: change it back!
		qDebug() << "f is even with c";
	} // kind of hack
	if (int(a*100)==int(i*100)) {
		a += 0.02;
		qDebug() << "a is even with i";
	} // kind of hack
	if (int(c*100)==int(i*100)) {
		c += 0.03;
		qDebug() << "a is even with i";
	} // kind of hack
	if (int(c*100)==int(a*100)) {
		c += 0.04;
		qDebug() << "a is even with i";
	} // kind of hack
	if (int(f*100)==int(i*100)) {
		i += 0.05;
		qDebug() << "a is even with i";
	} // kind of hack
	//if (a==c) { a += 0.01;}


	QPair <double, double> interSection;
	interSection = getIntersection(f,a, i, (f>=c) ? 1 : 0,  c,i, a, (f>=c) ? 1 : 0);
	z = interSection.first; y = interSection.second;

	interSection = getIntersection(c,i, (f>=c) ? 0 : 1,a,  f,a, (f>=c) ? 0 : 1,i);
	d = interSection.first; e = interSection.second;

	interSection = getIntersection(c,i, f,(a>=i) ? 0 : 1,  f,a, c, (a>=i) ? 0 : 1    );
	v = interSection.first; w = interSection.second;

	interSection = getIntersection(f,a, c,(a>=i) ? 1 : 0,  c,i, f, (a>=i) ? 1 : 0    );
	p = interSection.first; o = interSection.second;

	if (f>1) {f=1;} // also kind of hack
	if (a>1) {a=1; }
	if (c>1) {c=1;}
	if (i>1) {i=1; }
//	if (f<0) {f=0;}
//	if (a>0) {a=0; }

	QString parametersString;
	parametersString.sprintf("f: %.2f a: %.2f c: %.2f i: %.2f y: %.2f z: %.2f d: %.2f e: %.2f w: %.2f v: %.2f o: %.2f p: %.2f",
							 f, a, c, i, y, z, d, e, w, v, o, p );
	qDebug() << parametersString;
	sendToPerformers(parametersString);
	sendMainParameters(); // OSC to Processing
	emit newParameters(parametersString);



}

void WsServer::osc2ws(QString path, QList<QVariant> data)
{
	//qDebug()<< path << data;
	if (ws2osc_client) {
		QString messageString, values, typeString;

		foreach (QVariant chunk, data ) {
			if (chunk.type() == QVariant::Type::Int) {
				typeString += "i";
			} else if (chunk.type() == QVariant::Type::String) {
				typeString += "s";
			} else if (chunk.type() == QVariant::Type::Double ) {
				typeString += "d";
			} else if (chunk.type() == QVariant::Type::Bool ) {
				typeString += "b";

			} else {
				qDebug() << "Unknown type: " << chunk.typeName();
			}


			values += chunk.toString() + " ";

		}
		messageString = path + " " + typeString + " " + values;
		qDebug() << messageString;
		ws2osc_client->sendTextMessage(messageString);
	}
}


