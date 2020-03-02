#include "ClientQuery.h"
#include "Defencebutton.h"
#include <assert.h>
#include <QString>
#include <QMutexLocker>

//设置主机地址，连接信号槽
ClientQuery::ClientQuery(QObject *parent, int No,DefenceButton *b, QString hostAddr, int hostPort)
	:QUdpSocket(parent),defenceNo_(No),btn_(b),mtx_(),hostPort_(hostPort)
{
	hostAddr_.setAddress(hostAddr);
	connect(this, SIGNAL(bytesWritten(qint64)), this, SLOT(some_data_sended(qint64)));//参数是发送的字节数，每次将数据有效载荷写入设备的当前写通道时，都会发出此信号。
}

ClientQuery::~ClientQuery()
{
}

//通过传入参数返回defenceID
QByteArray ClientQuery::getDefenceID()
{
	QByteArray data = QString("%1").arg(defenceNo_).toAscii();
	return data;
}

//用UDPSOCKET的write函数将缓冲区的数据发送
void ClientQuery::some_data_sended(qint64)
{
	while (m_buffer_sending.empty()==false)//QList<QByteArray> m_buffer_sending
	{
		QByteArray & arraySending = *m_buffer_sending.begin();
		qint64 nTotalBytes = arraySending.size();
		qint64 nBytesWritten = writeDatagram(arraySending,hostAddr_,hostPort_);//返回发送的字节数
		if (nBytesWritten >=nTotalBytes)
		{
			m_buffer_sending.pop_front();//去除list的第一项，以达到循环发送的目的
		}
		else
			break;
	}
}

//将参数dtarray字符数组发送（缓冲区空则直接发送，否则存入缓冲区）
void ClientQuery::SendData(QByteArray dtarray)
{
	if (dtarray.size())
	{
		if (m_buffer_sending.empty()==true)//缓冲区空就直接发送
		{
			qint64 bytesWritten = writeDatagram(dtarray,hostAddr_,hostPort_);
			if (bytesWritten < dtarray.size())//发送失败就存到缓冲区
			{
				m_buffer_sending.push_back(dtarray);
			}
		}
		else//不空就存到缓冲区后面
		{
			m_buffer_sending.push_back(dtarray);
		}
	}	
}

void ClientQuery::setNormalStatus()
{
	{
		QMutexLocker locker(&mtx_);
		currTime_ = "";
	}
	btn_->SetDefenceStatus(DefenceStatus_normal);
}

void ClientQuery::appendWarnEvent(QString defenceID, QString currTime)
{
	{
		QMutexLocker locker(&mtx_);
		currTime_ = currTime;
		isDanger_ = false;
	}
	btn_->SetDefenceStatus(DefenceStatus_suspicous);
	btn_->appendWarnList(defenceID,currTime);
}

void ClientQuery::appendDangerEvent(QString defenceID, QString currTime)
{
	{
		QMutexLocker locker(&mtx_);
		currTime_ = currTime;
		isDanger_ = true;
	}
	btn_->SetDefenceStatus(DefenceStatus_alert);
	btn_->appendDangerList(defenceID,currTime);
}

bool ClientQuery::isEventHappen()
{
	QMutexLocker locker(&mtx_);
	return !(currTime_.isEmpty());
}

bool ClientQuery::isEventDanger()
{
	QMutexLocker locker(&mtx_);
	return isDanger_;
}

QString ClientQuery::getEventTime()
{
	QMutexLocker locker(&mtx_);
	return currTime_;
}