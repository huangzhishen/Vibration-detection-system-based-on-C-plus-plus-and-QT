#include "RequestDatagram.h"
#include "Config.h"
#include "Utility.h"
#include <QTime>
#include <QCoreApplication>

//构造函数造8个字节向量（数组）
RequestDatagram::RequestDatagram(QObject *parent):
	QObject(parent),udpSocket_(nullptr)
{
	channelData_.resize(8);//QVector<QByteArray> channelData_，哇，原来这里有个细微的地方，resize与reserve
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


//设置超时，当10s上一包数据还未传完，直接重新请求数据
int timeOut = 0;
char command[] = {0xC0,0xFF,0xD1,0x00,0x02,0x32,0x00,0x02,0x1C,0xC1};//请求数据命令

//定时器引发的信号emitGetData(QString,quint16)), 然后reqObj_调用槽函数SLOT(getData(QString,quint16))
//getData(QString ip,quint16 port)函数创建udpsocket对象，绑定端口，连接信号槽，有数据写入信号来，则调receiveData()
//注意这个emitGetData(QString,quint16))带参数，这个参数ip，port，从数据库提取，后面用writeDatagram，往这个地址写数据，所以这个地址是采集卡地址
void RequestDatagram::getData(QString ip,quint16 port)
{
	{
		QMutexLocker locker(&Utility::reqMtx);//初步了解这个锁主要是锁住isrequesting这个bool变量
		//说明正在请求，不重复请求
		if (Utility::isRequesting == true && timeOut < 5)//为啥要请求5次才能成功？400ms*5=2s
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
		udpSocket_->bind(Config::clientPort);//绑定主机的端口
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
	QUdpSocket * pSock = qobject_cast<QUdpSocket*>(sender());//找出哪一个QUdpSocket触发我这个槽函数的。
	
	QByteArray datagram;//拥于存放接收的数据报
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

		QThread::yieldCurrentThread();//由操作系统给到合适的进程处理一下语句

		pSock->writeDatagram(cmd,address,port_);
		count_++;
	}
	else
	{
		{
			QMutexLocker locker(&Utility::reqMtx);
			Utility::isRequesting = false;
		}
		emit emitReceivedData(channelData_);//信号所带的参数是
		for (int i = 0 ; i < 8 ; i++)
		{
			channelData_[i].clear();
			channelData_[i].reserve(10000);
		}
	}
}












