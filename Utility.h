#ifndef UTILITY_H
#define UTILITY_H
#include <QObject>
#include <QString>
#include <QMutex> 

enum DefenceType
{
	DefenceType_1=0,
	DefenceType_2=1,
	DefenceType_3=2,
	DefenceType_4=3,
	DefenceType_5=4
};

enum DefenceStatus
{
	DefenceStatus_normal=0,	   //正常运行
	DefenceStatus_alert=1,	   //危险
	DefenceStatus_select=2,	   //选中
	DefenceStatus_suspicous=3, //可疑
	DefenceStatus_init=4,	   //未启动
};

class Utility:public QObject{
public:
	static void ShowMessageBoxError(const char* info);
	static int ShowMessageBoxQuesion(const char* info);
	static void ShowMessageBoxInfo(const char* info);

	static bool isFileExist(QString &fileName);
	static bool isHostExist(QString &hostID);
	static QString getFileName(QString filter);
	static QString getFileNameWithExt(QString fileName);
	static bool copyFile(QString srcFile, QString dstFile);

	static void setStyle();
	static void moveToCenter(QWidget *widget, int deskWidth, int deskHeight);
	static void setFixSize(QWidget *widget);
	static void setWindowsMax(QWidget *widget);

	static void insertIntoDb(QString dataTime,QString areaNo, QString defenceType, QString hostNo, QString hostName, QString strData);



	static DefenceType getDefenceType(int &defencetype);
	static DefenceStatus getDefenceStatus(QString &defenceStatus);
	
	//设置不同的互斥锁，提高并发，数据库锁
	static QMutex mutex;  
	static bool isRequesting;
	static QMutex reqMtx;
	static bool isMainWidget;			//判断按键是在主窗口还是在设置窗口
};



#endif


