#include "securitysystem.h"
#include <QApplication>
#include <QTextCodec>
#include <QDesktopWidget>
#include "Config.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QString>
#include <QSqlQuery>
#include <QMap>
#include "Utility.h"
#include "SystemDB.h"
#include "UserLogin.h"
#include "SystemSetting.h"
#include "HostSetting.h"
#include "DefenceSetting.h"
#include "DataQuery.h"
#include "CheckData.h"
#include "ClientQuery.h"
#include "PickFeature.h"
#include <QDebug>
#include <fstream>
#include <vector>
using namespace std;

//数据库操作加锁
//添加日志记录方便后期诊断信号,将qDebug处的信心换掉

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Utility::setStyle();

	QTextCodec *codec = QTextCodec::codecForName("gb18030");
	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);


	Config::AppPath=QApplication::applicationDirPath()+"/";
	Config::DeskWidth=qApp->desktop()->availableGeometry().width();
	Config::DeskHeigth=qApp->desktop()->availableGeometry().height()-40;
	Config::ReadConfig();//读取配置文件信息

//	Config::ReadConfigchinese();

	SystemDB db("QMYSQL","localhost","SecuritySystem","root","123456");
	if(!db.OpenDB()) 
	{  
		Utility::ShowMessageBoxError((const char*)"数据库连接失败");
		return -1;
	}

	UserLogin d;
	d.show();

	/*ifstream file("E:\\testData\\fenggao.csv",ios::in);
	vector<float> data;
	string line;
	while (getline(file,line)){
		QString str(line.c_str());
		data.push_back(str.toFloat()/4095*5.0);
	}
	file.close();
	float zcr,amp;
	PickFeature::getZCRandAmp(data,zcr,amp);
	qDebug() << zcr << " " << amp;*/



	//SystemSetting w;
	//w.show();

	/*HostSetting w;
	w.show();*/
	
	//DefenceSetting w;
	//w.show();

	//DataQuery w;
	//w.show();
	

/*	QMap<QString,QString> mapEvents;
	mapEvents["2017-10-31 16:29:34"] = "001";
	CheckData w(mapEvents);
	w.show();
*/
	/*
	float data[5000];
	QTime time;  
	time.start();  
	for (int i = 0 ; i < 5000 ; i++)
	{
		data[i] = 2.756958;
	}
	qDebug()<<time.elapsed()/1000.0<<"s";  
	Utility::insertIntoDb("001","1","001","host",data);
	*/
	/*
	query.exec("select * from dataInfo");
	float data2[5000];
	while (query.next())
	{	
		QString strData2 = query.value(6).toString();
		QStringList strList = strData2.split(",");
		for (int i = 0 ; i < strList.size() ; i++)
		{
			data2[i] = strList[i].toFloat();
		}
	}

	qDebug()<<time.elapsed()/1000.0<<"s";  
	*/
	
	return a.exec();
}
