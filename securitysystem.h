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
#include <memory>//����ָ��
#include "mmtimer.h"
#include "win_qextserialport.h"   //����ͷ�ļ�

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
	//���������
	//ϵͳ����
	void on_systemAction_triggered();//��������
	void on_hostManageAction_triggered();//��������
	void on_defenceAction_triggered();//��Ϣ��ѯ
	void on_queryAction_triggered();
	void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void on_btnStartMoniter_clicked();//ȫ����������
	void on_cbRecordAll_stateChanged(int state);

	void process_finished(QObject *obj);

	void resetNotice(int ID);
	void showDateTime();       
	
	void new_data_recieved(QVector<QByteArray>);//�յ�����
	void display_error(QAbstractSocket::SocketError);//������Ϣ��ʾ
	
	void sendMsgtoServer();
	void readComMessage();  //�ۺ�������

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
	QMap<int,DefenceButton*> mapDefenceBtn_;//һ��ӳ������ݽṹ���൱�����飬������ǰ��
	QMap<int,QLabel*> mapDefenceLabel_;

	TaskThreadPool *pTaskThreadPool_;

	QLabel *labWelcome_;  //��ӭ��ǩ
	QLabel *labUser_;    //��ǰ�û���ǩ
	QLabel *labLive_;    //������ʱ���ǩ
	QLabel *labTime_;    //��ǰʱ���ǩ
	QTimer *timerDate_;  //��������ʱ������ʾ��ǰʱ�䶨ʱ������   

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


	Win_QextSerialPort *Com_;//�������ڶ���

	//����ָ��
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
