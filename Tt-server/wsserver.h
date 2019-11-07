#ifndef WSSERVER_H
#define WSSERVER_H

#include <QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QStringList>

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
	void setPaused(bool onOff);
	VoteResults getStatistics(int card, int column); // TODO: return stuct (?QPair) { int count; double sum; }
	void analyze(int card);
	void calculateParameters(double f, double a, double c, double i);
	QPair<double, double> getIntersection(double d1_x1, double d1_y1, double d1_x2, double d1_y2,
	  double d2_x1, double d2_y1, double d2_x2, double d2_y2);
	int currentCard;




Q_SIGNALS:
    void closed();
    void newConnection(int connectionsCount);
	void newMessage(QString eventString);
	void newParameters(QString parametersString);



private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(QString message);
    void socketDisconnected();


private:
    QWebSocketServer *m_pWebSocketServer;
	QList<QWebSocket *> m_clients;
	QList<QWebSocket *> performers;
	QWebSocket * csoundClient;
	int currentClient;
	bool paused;
	QHash <QString, QVector<double>> resultsHash;

};



#endif // WSSERVER_H
