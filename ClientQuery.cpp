#include "ClientQuery.h"
#include "Defencebutton.h"
#include <assert.h>
#include <QString>
#include <QMutexLocker>

//����������ַ�������źŲ�
ClientQuery::ClientQuery(QObject *parent, int No,DefenceButton *b, QString hostAddr, int hostPort)
	:QUdpSocket(parent),defenceNo_(No),btn_(b),mtx_(),hostPort_(hostPort)
{
	hostAddr_.setAddress(hostAddr);
	connect(this, SIGNAL(bytesWritten(qint64)), this, SLOT(some_data_sended(qint64)));//�����Ƿ��͵��ֽ�����ÿ�ν�������Ч�غ�д���豸�ĵ�ǰдͨ��ʱ�����ᷢ�����źš�
}

ClientQuery::~ClientQuery()
{
}

//ͨ�������������defenceID
QByteArray ClientQuery::getDefenceID()
{
	QByteArray data = QString("%1").arg(defenceNo_).toAscii();
	return data;
}

//��UDPSOCKET��write�����������������ݷ���
void ClientQuery::some_data_sended(qint64)
{
	while (m_buffer_sending.empty()==false)//QList<QByteArray> m_buffer_sending
	{
		QByteArray & arraySending = *m_buffer_sending.begin();
		qint64 nTotalBytes = arraySending.size();
		qint64 nBytesWritten = writeDatagram(arraySending,hostAddr_,hostPort_);//���ط��͵��ֽ���
		if (nBytesWritten >=nTotalBytes)
		{
			m_buffer_sending.pop_front();//ȥ��list�ĵ�һ��Դﵽѭ�����͵�Ŀ��
		}
		else
			break;
	}
}

//������dtarray�ַ����鷢�ͣ�����������ֱ�ӷ��ͣ�������뻺������
void ClientQuery::SendData(QByteArray dtarray)
{
	if (dtarray.size())
	{
		if (m_buffer_sending.empty()==true)//�������վ�ֱ�ӷ���
		{
			qint64 bytesWritten = writeDatagram(dtarray,hostAddr_,hostPort_);
			if (bytesWritten < dtarray.size())//����ʧ�ܾʹ浽������
			{
				m_buffer_sending.push_back(dtarray);
			}
		}
		else//���վʹ浽����������
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