#ifndef SECURITYSYSTEM_H
#define SECURITYSYSTEM_H

#include <QtGui/QMainWindow>
#include <QTcpSocket>
#include <QMap>
#include <QDateTime>
#include <QLabel>
#include <QBitmap>
#include "ui_securitysystem.h"
#include "DataProcess.h"
#include "ClientQuery.h"
#include "TaskThreadPool.h"
#include "RequestDatagram.h"
#include <vector>
#include <memory>//智能指针
#include "mmtimer.h"
#include "win_qextserialport.h"   //串口头文件

class SecuritySystem : public QMainWindow
{
	Q_OBJECT

public:
	SecuritySystem(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SecuritySystem();
	QString SecuritySystem::stringToUnicode(QString str);
	virtual void timerEvent(QTimerEvent * evt);

signals:
	void do_task(QObject *, const QByteArray &);
	void emitGetData(QString,quint16);
	
private slots:
	//快捷栏设置
	//系统设置
	void on_systemAction_triggered();//主机设置
	void on_hostManageAction_triggered();//防区设置
	void on_defenceAction_triggered();//信息查询
	void on_queryAction_triggered();
	void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void on_btnStartMoniter_clicked();//全部启动监听
	void on_cbRecordAll_stateChanged(int state);

	void process_finished(QObject *obj);

	void resetNotice(int ID);
	void showDateTime();       
	
	void new_data_recieved(QVector<QByteArray>);//收到数据
	void display_error(QAbstractSocket::SocketError);//错误信息显示
	
	void sendMsgtoServer();
	void readComMessage();  //槽函数声明

    void AutoSend1();
	void AutoSend2();
	void AutoSend3();
	void AutoSend4();
	void AutoSend5();
	void AutoSend6();
    void AutoSend7();
	void AutoSend8();

	/*
	void AutoSendw1();
	void AutoSendw2();
	void AutoSendw3();
	void AutoSendw4();
	void AutoSendw5();
	void AutoSendw6();
	void AutoSendw7();
	void AutoSendw8();
	*/



private:
	Ui::SecuritySystemClass ui_;
	DataProcess *pdataProcess_;
	bool isStart_;
	int Timer_;

	QMap<ClientQuery *, QDateTime> tcpClients_;
	QSet<int> clientID_;
	QMap<int,DefenceButton*> mapDefenceBtn_;//一种映射的数据结构，相当于数组，索引是前者
	QMap<int,QLabel*> mapDefenceLabel_;

	TaskThreadPool *pTaskThreadPool_;

	QLabel *labWelcome_;  //欢迎标签
	QLabel *labUser_;    //当前用户标签
	QLabel *labLive_;    //已运行时间标签
	QLabel *labTime_;    //当前时间标签
	QTimer *timerDate_;  //计算在线时长和显示当前时间定时器对象   

	QStringList defenceMap_;
	QPixmap pixmap_;
	QPalette palette_;
	QByteArray dataCache_;
	mmtimer *highPriTimer_;
	QThread *thread_;
	RequestDatagram *reqObj_;

	QTimer *mytime1;
	QTimer *mytime2;
	QTimer *mytime3;
	QTimer *mytime4;
	QTimer *mytime5;
	QTimer *mytime6;
	QTimer *mytime7;
	QTimer *mytime8;

/*	QTimer *mytimew1;
	QTimer *mytimew2;
	QTimer *mytimew3;
	QTimer *mytimew4;
	QTimer *mytimew5;
	QTimer *mytimew6;
	QTimer *mytimew7;
	QTimer *mytimew8;
*/


	Win_QextSerialPort *Com_;//声明串口对象

	//智能指针
	std::vector<std::shared_ptr<QTableWidgetItem>> itemTime_;
	std::vector<std::shared_ptr<QTableWidgetItem>> itemArea_;
	std::vector<std::shared_ptr<QTableWidgetItem>> itemMessage_;
	void initForm();
	void initTableWidget();
	void initTreeWidget();
	void initDefenceButton();
	void setDefenceButtonVisible();
};

#endif // SECURITYSYSTEM_H
