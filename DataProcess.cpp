#include "DataProcess.h"
#include <qDebug>
#include <QThread>
#include <QTime>
#include <windows.h>
#include <QtCore/QCoreApplication>
#include "Utility.h"
#include "Config.h"
#include "ClientQuery.h"
#include "PickFeature.h"
#include <QFileInfo>
#include <opencv2/opencv.hpp>
using namespace cv; 

DataProcess::DataProcess(QObject *parent)
	:ThreadTask(parent)
{
}


DataProcess::~DataProcess(void)
{
}

CvANN_MLP *BP[8] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
bool bpExist[8] = {true,true,true,true,true,true,true,true};

int testDataProcessFunc(const vector<float> &processData , int areaNo)
{
	qDebug() << "data size = " << processData.size() << ", areaNo = " << areaNo;
	if (BP[areaNo-1] == nullptr && bpExist[areaNo-1])
	{
		QFileInfo fileinfo(QString("bpnet%1.xml").arg(areaNo));
		if (fileinfo.exists() == false)
		{
			qDebug() << QObject::tr("防区%1BP模型不存在，请训练后在开始。").arg(areaNo);
			bpExist[areaNo-1] = false;
			return 1;
		}
		else{
			BP[areaNo-1] = new CvANN_MLP();
			BP[areaNo-1]->load(QString("bpnet%1.xml").arg(areaNo).toAscii().data());
		}
	}
	if (bpExist[areaNo-1] == false)
	{
		qDebug() << QObject::tr("防区%1BP模型不存在，请训练后在开始。").arg(areaNo);
		return 1;
	}
	
	vector<float> testData;
	PickFeature::getFeature(processData,testData);

	int predictLabel;
	Mat testDataMat(1, 7, CV_32FC1, testData.data());
	Mat responseMat;    

	//CvANN_MLP bp;//bp神经网络 
	////优化 只打开一次
	//bp.load(QString("bpnet%1.xml").arg(areaNo).toAscii().data());
//	bp.predict(testDataMat,responseMat);
	BP[areaNo-1]->predict(testDataMat,responseMat);
	Point maxloc;
	minMaxLoc(responseMat,NULL,NULL,NULL,&maxloc);
	predictLabel = maxloc.x+1;
	qDebug() << "Thread: " << (quintptr)QThread::currentThreadId() << " AreaNo: " <<  areaNo << " Result :"<< predictLabel ;

	return predictLabel;

}

//这个函数在下面的run函数中调用
//其实这个可以看成复杂数据处理的一个类，run函数完成全部工作
int judgeEvent(const vector<float> &processData , int areaNo)
{
	vector<float> data(processData);
	float zcr,amp;
	PickFeature::getZCRandAmp(data,zcr,amp);
	qDebug() << "AreaNo: " <<  areaNo  << " zcr: "<<zcr << " amp: " << amp;
	int result = 1;
	switch (areaNo)
	{
	case 1 :
		if (zcr >= Config::zw1 && amp>=Config::ew1)
		{
			if (zcr >= Config::zd1 && amp >= Config::ed1)
			{
				result = 3;
			}
			else
			{
				result = 2;
			}
		}
		else
		{
			result = 1;
		}
		return result;break;
	case 2 :
		if (zcr >= Config::zw2 && amp>=Config::ew2)
		{
			if (zcr >= Config::zd2 && amp >= Config::ed2)
			{
				result = 3;
			}
			else
			{
				result = 2;
			}
		}
		else
		{
			result = 1;
		}
		return result;break;
	case 3 :
		if (zcr >= Config::zw3 && amp>=Config::ew3)
		{
			if (zcr >= Config::zd3 && amp >= Config::ed3)
			{
				result = 3;
			}
			else
			{
				result = 2;
			}
		}
		else
		{
			result = 1;
		}
		return result;break;
	case 4 :
		if (zcr >= Config::zw4 && amp>=Config::ew4)
		{
			if (zcr >= Config::zd4 && amp >= Config::ed4)
			{
				result = 3;
			}
			else
			{
				result = 2;
			}
		}
		else
		{
			result = 1;
		}
		return result;break;
	case 5 :
		if (zcr >= Config::zw5 && amp>=Config::ew5)
		{
			if (zcr >= Config::zd5 && amp >= Config::ed5)
			{
				result = 3;
			}
			else
			{
				result = 2;
			}
		}
		else
		{
			result = 1;
		}

		return result;
		break;
	case 6 :
		if (zcr >= Config::zw6 && amp>=Config::ew6)
		{
			if (zcr >= Config::zd6 && amp >= Config::ed6)
			{
				result = 3;
			}
			else
			{
				result = 2;
			}
		}
		else
		{
			result = 1;
		}
		return result;break;
	case 7 :
		if (zcr >= Config::zw7 && amp>=Config::ew7)
		{
			if (zcr >= Config::zd7 && amp >= Config::ed7)
			{
				result = 3;
			}
			else
			{
				result = 2;
			}
		}
		else
		{
			result = 1;
		}
		return result;break;
	case 8 :
		if (zcr >= Config::zw8 && amp>=Config::ew8)
		{
			if (zcr >= Config::zd8 && amp >= Config::ed8)
			{
				result = 3;
			}
			else
			{
				result = 2;
			}
		}
		else
		{
			result = 1;
		}
		return result;break;
	}

}



//哦，666，明白了
void DataProcess::run(QObject * task_source, const QByteArray & data_array)
{
	ClientQuery *clientSrc = static_cast<ClientQuery*>(task_source);
	//qDebug() << "DataProcess::run " <<  (quintptr)QThread::currentThreadId() << ", defenceID " <<  clientSrc->getDefenceID()  <<", data size :" << data_array.size();
	QDateTime currentDataTime = QDateTime::currentDateTime();
	QString currTime = currentDataTime.toString("yyyy-MM-dd hh:mm:ss");


	//传递给主线程用于波形图
	int originData[10000] = {0};
	//传递模型进行判断
	vector<float> processData;
	processData.reserve(10000);
	//存储到数据库
	QString dbString;
	dbString.reserve(50000);

	uchar data1;
	uchar data2;
	for (int i = 0 ; i < 10000; i++)
	{
		data1 = data_array[2*i];
		data2 = data_array[2*i + 1];
		originData[i] = data1 * 256 + data2;//666呀，这里乘上256，真的很巧妙！
		dbString.push_back(QString("%1,").arg(originData[i]));
		processData.push_back(originData[i] * 5.0 / 4095);	//*5.0变成浮点数！
	}

	int ret =judgeEvent(processData,clientSrc->getDefenceID().toInt()); //testDataProcessFunc(processData,clientSrc->getDefenceID().toInt());
	if (ret == 1)
	{
		clientSrc->setNormalStatus();	
	}
	else if (ret == 2)
	{
		clientSrc->appendWarnEvent(clientSrc->getDefenceID(),currTime);
	}
	else
	{
		clientSrc->appendDangerEvent(clientSrc->getDefenceID(),currTime);
	}

	if (ret == 1)
	{
		if (Config::isRecordAllData)
		{
			Utility::insertIntoDb(currTime,clientSrc->getDefenceID(),QString("%1").arg(ret),"001","host",dbString);
		}
	}
	else
	{
		Utility::insertIntoDb(currTime,clientSrc->getDefenceID(),QString("%1").arg(ret),"001","host",dbString);
	}

}