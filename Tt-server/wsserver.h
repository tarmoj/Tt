#ifndef WSSERVER_H
#define WSSERVER_H

#include <QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)


class WsServer : public QObject
{
    Q_OBJECT
public:
    explicit WsServer(quint16 port, QObject *parent = NULL);
    ~WsServer();

	void sendMessage(QWebSocket *socket, QString message);
	void send2all(QString message);
	void setPaused(bool onOff);

Q_SIGNALS:
    void closed();
    void newConnection(int connectionsCount);
	void newMessage(QString eventString);



private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(QString message);
    void socketDisconnected();


private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
	QWebSocket * csoundClient;
	int currentClient;
	bool paused;
    QHash <QString, QVector<double>> resultsHash;
};



#endif // WSSERVER_H
