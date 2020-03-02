#ifndef CLIENTQUERY_H
#define CLIENTQUERY_H

#include <QUdpSocket>
#include <QList>
#include <QMutex>
#include "Defencebutton.h"

class ClientQuery: public QUdpSocket
{
	Q_OBJECT
public:
	ClientQuery(QObject *parent, int No, DefenceButton *b, QString hostAddr, int hostPort);
	~ClientQuery();
	QByteArray getDefenceID();
	void setNormalStatus();
	void appendWarnEvent(QString defenceID, QString currTime);
	void appendDangerEvent(QString defenceID, QString currTime);
	bool isEventHappen();
	bool isEventDanger();
	QString getEventTime();

private:
	int defenceNo_;
	QList<QByteArray> m_buffer_sending;
	DefenceButton *btn_;
	QMutex mtx_;
	QString currTime_;
	QHostAddress hostAddr_;
	int hostPort_;
	bool isDanger_;
public slots:
	void some_data_sended(qint64);
	void SendData(QByteArray dtarray);
};


#endif

