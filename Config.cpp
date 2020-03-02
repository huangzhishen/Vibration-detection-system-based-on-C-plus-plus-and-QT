#include "Config.h"
#include "Utility.h"
#include <QSettings>
#include "SystemSetting.h"


bool Config::isMove = false;
QString Config::AppPath = "";

int Config::DeskWidth = 0;
int Config::DeskHeigth = 0;

QString Config::CurrentImage="";

int Config::timeGap = 1000;
int Config::defenceCount = 8;

bool Config::isRecordAllData = false;


QString Config::lastLoginUser = "admin";
QString Config::currentUserName = "admin";
QString Config::currentUserPassword = "";
QString Config::currentUserType = "";

QString Config::softwareTitle = "Security System";


int Config::defenceSize = 35;

QString Config::hostAddr = "211.66.31.250";
int Config::hostPort = 10000;
int Config::clientPort = 6789;

int Config::defenceBGWidth = 1580;
int Config::defenceBGHeight = 850;
int Config::defenceOffsetX = 10;
int Config::defenceOffsetY = 15;

int Config::labelWidth = 0;
int Config::labelHeight = 0;

int Config::labelOffsetX = 0;
int Config::labelOffsetY = 0;

float Config::warnZcr = 0.0;
float Config::warnAmp = 0.0;
float Config::dangerZcr = 0.0;
float Config::dangerAmp =0.0;

//短信内容：
QString Config::contactName;
QString Config::contactTel;
QString Config::contactMessage;
QString Config::contactMessage1;


float Config::zw1;
float Config::zd1;
float Config::ew1;
float Config::ed1;
float Config::zw2;
float Config::zd2;
float Config::ew2;
float Config::ed2;
float Config::zw3;
float Config::zd3;
float Config::ew3;
float Config::ed3;
float Config::zw4;
float Config::zd4;
float Config::ew4;
float Config::ed4;
float Config::zw5;
float Config::zd5;
float Config::ew5;
float Config::ed5;
float Config::zw6;
float Config::zd6;
float Config::ew6;
float Config::ed6;
float Config::zw7;
float Config::zd7;
float Config::ew7;
float Config::ed7;
float Config::zw8;
float Config::zd8;
float Config::ew8;
float Config::ed8;



void Config::ReadConfig()
{
	QString fileName = AppPath + "Config.ini";
	if(!Utility::isFileExist(fileName))
	{
		return;
	}
	QSettings configFile(fileName,QSettings::IniFormat);
	configFile.beginGroup("AppConfig");

	Config::defenceSize = configFile.value("defenceSize").toInt();
	Config::defenceBGWidth = configFile.value("defenceBGWidth").toInt();
	Config::defenceBGHeight = configFile.value("defenceBGHeight").toInt();
	Config::defenceOffsetX = configFile.value("defenceOffsetX").toInt();
	Config::defenceOffsetY = configFile.value("defenceOffsetY").toInt();

	Config::labelWidth = configFile.value("labelWidth").toInt();
	Config::labelHeight = configFile.value("labelHeight").toInt();
	Config::labelOffsetX = configFile.value("labelOffsetX").toInt();
	Config::labelOffsetY = configFile.value("labelOffsetY").toInt();

	Config::warnZcr = configFile.value("warningZcr").toFloat();
	Config::warnAmp = configFile.value("warningAmp").toFloat();
	Config::dangerZcr = configFile.value("dangerZcr").toFloat();
	Config::dangerAmp = configFile.value("dangerAmp").toFloat();

	Config::zw1=configFile.value("ew1").toFloat();
	Config::zd1=configFile.value("zd1").toFloat();
	Config::ew1=configFile.value("ew1").toFloat();
	Config::ed1=configFile.value("ed1").toFloat();
	Config::zw2=configFile.value("zw2").toFloat();
	Config::zd2=configFile.value("zd2").toFloat();
	Config::ew2=configFile.value("ew2").toFloat();
	Config::ed2=configFile.value("ed2").toFloat();
	Config::zw3=configFile.value("zw3").toFloat();
	Config::zd3=configFile.value("zd3").toFloat();
	Config::ew3=configFile.value("ew3").toFloat();
	Config::ed3=configFile.value("ed3").toFloat();
    Config::zw4=configFile.value("zw4").toFloat();
	Config::zd4=configFile.value("zd4").toFloat();
	Config::ew4=configFile.value("zw4").toFloat();
	Config::ed4=configFile.value("ed4").toFloat();
	Config::zw5=configFile.value("ew5").toFloat();
	Config::zd5=configFile.value("zd5").toFloat();
	Config::ew5=configFile.value("ew5").toFloat();
	Config::ed5=configFile.value("ed5").toFloat();
	Config::zw6=configFile.value("zw6").toFloat();
	Config::zd6=configFile.value("zd6").toFloat();
	Config::ew6=configFile.value("ew6").toFloat();
	Config::ed6=configFile.value("ed6").toFloat();
	Config::zw7=configFile.value("zw7").toFloat();
	Config::zd7=configFile.value("zd7").toFloat();
	Config::ew7=configFile.value("ew7").toFloat();
	Config::ed7=configFile.value("ed7").toFloat();
	Config::zw8=configFile.value("zw8").toFloat();
	Config::zd8=configFile.value("zd8").toFloat();
	Config::ew8=configFile.value("ew8").toFloat();
	Config::ed8=configFile.value("ed8").toFloat();

	Config::contactTel = configFile.value("contactingTel").toString();

	//configFile.setIniCodec("UTF8");//设置为GB2312即可  
	Config::contactName = configFile.value("contactingName").toString();
	//configFile.setIniCodec("UTF8");//设置为GB2312即可  
	Config::contactMessage = configFile.value("contactingMessage").toString();
	//configFile.setIniCodec("UTF8");//设置为GB2312即可  
	Config::contactMessage1 = configFile.value("contactingMessage1").toString();
	configFile.endGroup();

}

/*
void Config::ReadConfigchinese()
{
	QString fileName1 = AppPath + "Config1.ini";
	QSettings configFile1(fileName1,QSettings::IniFormat);

	configFile1.beginGroup("AppConfig");
	configFile1.setIniCodec("UTF8");//设置为GB2312即可  
	Config::contactName = configFile1.value("contactingName").toString();
	configFile1.setIniCodec("UTF8");//设置为GB2312即可  
	Config::contactMessage = configFile1.value("contactingMessage").toString();
	configFile1.setIniCodec("UTF8");//设置为GB2312即可  
	Config::contactMessage1 = configFile1.value("contactingMessage1").toString();
	configFile1.endGroup();
}
*/
void Config::WriteConfig()
{
	QString fileName = AppPath + "Config.ini";
	QSettings configFile(fileName, QSettings::IniFormat);

	configFile.beginGroup("AppConfig");

	configFile.setValue("defenceSize",Config::defenceSize);
	
	/*
	configFile.setValue("contactName",ui_.leName->text());  
	configFile.setValue("contactTel",ui_.leTel->text());  
	configFile.setValue("contactMessage",ui_.leMsg->text());  
	configFile.setValue("contactMessage1",ui_.leMsgg->text());  
	*/

	configFile.endGroup();
}