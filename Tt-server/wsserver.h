#ifndef WSSERVER_H
#define WSSERVER_H

#include <QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QStringList>

#include "qosc/qoscclient.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

struct VoteResults {
	int count;
	double sum;
	double average;
} ;

class WsServer : public QObject
{
    Q_OBJECT
public:
	explicit WsServer(quint16 port, QObject *parent = nullptr);
    ~WsServer();

	void sendMessage(QWebSocket *socket, QString message);
	void send2all(QString message);
	void sendToPerformers(QString message);
	void sendToVoters(QString message);
	void setPaused(bool onOff);
    VoteResults getStatistics(int card, int column);
    void analyze(int card);
	void calculateParameters();
	QPair<double, double> getIntersection(double d1_x1, double d1_y1, double d1_x2, double d1_y2,
	  double d2_x1, double d2_y1, double d2_x2, double d2_y2);
	void setOscAddress(QString host, quint16 port);
	void sendMainParameters(); // sends f,a,c,i to osc address (for Processing)
	void emulate(); // set random values to  f,a,c,i
    void sendReset();
    int currentCard;
    bool useManualEntry;
    int votersCount;




    void enterResults(int column, QString dataString);

Q_SIGNALS:
    void closed();
    void newConnection(int connectionsCount);
	void newMessage(QString eventString);
	void newParameters(QString parametersString);



private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(QString message);
    void socketDisconnected();
	void osc2ws(QString path, QList<QVariant> data);


private:
    QWebSocketServer *m_pWebSocketServer;
	QList<QWebSocket *> m_clients;
	QList<QWebSocket *> performers, voters;
	QWebSocket * csoundClient;
	int currentClient;
	bool paused;
	QHash <QWebSocket *, QVector<double>> resultsHash;
	double f,a,c,i,y,z, e, d, w, v, o, p;
	QOscClient * m_oscAddress;
	QWebSocket * ws2osc_client;
    QVector< QVector <double> > dataMatrix; //for manual data entries -  0 -  f data, 1 -  a, 2 - c, 3 - i;


};



#endif // WSSERVER_H
