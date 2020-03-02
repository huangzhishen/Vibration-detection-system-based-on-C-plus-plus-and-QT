#include "RequestDatagram.h"
#include "Config.h"
#include "Utility.h"
#include <QTime>
#include <QCoreApplication>

//���캯����8���ֽ����������飩
RequestDatagram::RequestDatagram(QObject *parent):
	QObject(parent),udpSocket_(nullptr)
{
	channelData_.resize(8);//QVector<QByteArray> channelData_���ۣ�ԭ�������и�ϸ΢�ĵط���resize��reserve
	for(int i = 0 ; i < 8; i++)
	{
		channelData_[i].reserve(10000);
	}
}


RequestDatagram::~RequestDatagram()
{
	if (udpSocket_)
	{
		delete udpSocket_;
	}
}


//���ó�ʱ����10s��һ�����ݻ�δ���ֱ꣬��������������
int timeOut = 0;
char command[] = {0xC0,0xFF,0xD1,0x00,0x02,0x32,0x00,0x02,0x1C,0xC1};//������������

//��ʱ���������ź�emitGetData(QString,quint16)), Ȼ��reqObj_���òۺ���SLOT(getData(QString,quint16))
//getData(QString ip,quint16 port)��������udpsocket���󣬰󶨶˿ڣ������źŲۣ�������д���ź��������receiveData()
//ע�����emitGetData(QString,quint16))���������������ip��port�������ݿ���ȡ��������writeDatagram���������ַд���ݣ����������ַ�ǲɼ�����ַ
void RequestDatagram::getData(QString ip,quint16 port)
{
	{
		QMutexLocker locker(&Utility::reqMtx);//�����˽��������Ҫ����סisrequesting���bool����
		//˵���������󣬲��ظ�����
		if (Utility::isRequesting == true && timeOut < 5)//ΪɶҪ����5�β��ܳɹ���400ms*5=2s
		{
			timeOut++;
			return;
		}
		else
		{
			timeOut=0;
			Utility::isRequesting = true;
		}
	}	
	if (udpSocket_ == nullptr)
	{
		udpSocket_ = new QUdpSocket();
		udpSocket_->bind(Config::clientPort);//�������Ķ˿�
		connect(udpSocket_,SIGNAL(readyRead()),this,SLOT(receiveData()),Qt::DirectConnection);
	}

	QByteArray datagram;
	datagram.append(command,10);
	QHostAddress address;
	address.setAddress(ip);
	ip_ = ip;
	port_ = port;
	count_ = 0;
	udpSocket_->writeDatagram(datagram,address,port);
}

int len = 0;
void RequestDatagram::receiveData()
{
	QUdpSocket * pSock = qobject_cast<QUdpSocket*>(sender());//�ҳ���һ��QUdpSocket����������ۺ����ġ�
	
	QByteArray datagram;//ӵ�ڴ�Ž��յ����ݱ�
	do{
		datagram.resize(pSock->pendingDatagramSize());
		pSock->readDatagram(datagram.data(),datagram.size());
	}while(pSock->hasPendingDatagrams());

	for (int i = 0 ; i < datagram.size(); i += 16)
	{
		for (int j = 0 ; j < 8; j++)
		{
			channelData_[j].push_back(datagram[i+2*j]);
			channelData_[j].push_back(datagram[i+2*j+1]);
		}
	}

	if ( count_ < 100  )
	{
		QByteArray cmd;
		cmd.append(command,10);
		QHostAddress address;
		address.setAddress(ip_);

		QThread::yieldCurrentThread();//�ɲ���ϵͳ�������ʵĽ��̴���һ�����

		pSock->writeDatagram(cmd,address,port_);
		count_++;
	}
	else
	{
		{
			QMutexLocker locker(&Utility::reqMtx);
			Utility::isRequesting = false;
		}
		emit emitReceivedData(channelData_);//�ź������Ĳ�����
		for (int i = 0 ; i < 8 ; i++)
		{
			channelData_[i].clear();
			channelData_[i].reserve(10000);
		}
	}
}












