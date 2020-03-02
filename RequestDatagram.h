#ifndef REQUESTDATAGRAM_H
#define REQUESTDATAGRAM_H
#include <QObject>
#include <QByteArray>
#include <QThread>
#include <QtNetwork/QUdpSocket>
#include <QVector>
class RequestDatagram:public QObject
{
	Q_OBJECT
public:
	RequestDatagram(QObject *parent = 0);
	~RequestDatagram();

	public slots:
	void getData(QString,quint16);
	void receiveData();
signals:
	void emitReceivedData(QVector<QByteArray>);

private:
	int count_;
	QUdpSocket *udpSocket_;
	QString ip_;
	quint16 port_;
	QVector<QByteArray> channelData_;
};
#endif

