#ifndef DefenceButton_H
#define DefenceButton_H

#include <QWidget>
#include <QTimer>
#include <QMap>
#include <QMutex>
#include <QPixmap>
#include <QLabel>
#include "Utility.h"


class DefenceButton: public QWidget
{
    Q_OBJECT
public:

    explicit DefenceButton(QWidget *parent=0);//explicit关键字

    explicit DefenceButton(QWidget *parent, QString defenceID, DefenceType defenceType, QString hostID, QString hostName,  
		QString defenceSound,QString defenceImage, QString defenceUser, int defenceX, int defenceY, DefenceStatus defenceStatus);


    void SetDefenceID(QString defenceID);                       //设置防区ID
    void SetDefenceStatus(DefenceStatus defenceStatus);         //设置防区状态,例如防区报警、防区旁路等
	void setHostAddrAndPort(QString hostAddr,int hostPort);
	QString getHostAddr();
	int getHostPort();
    QString GetDefenceID()const{return defenceID;}              //获取防区ID
    DefenceType GetDefenceType()const{return defenceType;}      //获取防区类型
    QString GetHostID()const{return hostID;}                    //获取防区对应主机ID
    QString GetHostName()const{return hostName;}                //获取防区对应主机名称

    QString GetDefenceSound()const{return defenceSound;}        //获取防区报警声音
    QString GetDefenceImage()const {return defenceImage;}       //获取防区对应地图文件名称
    QString GetDefenceUser()const{return defenceUser;}          //获取防区用户
    int GetDefenceX()const{return defenceX;}                    //获取防区X坐标
    int GetDefenceY()const{return defenceY;}                    //获取防区Y坐标
    DefenceStatus GetDefenceStatus()const{return defenceStatus;}//获取防区状态

	void appendWarnList(QString defenceID,QString currTime);
	void appendDangerList(QString defenceID,QString currTime);

	int getWarningCount() { return warnningList_.size(); }
	int getDangerCount() { return dangerList_.size(); }

signals:
	void sig_resetList(int ID);
protected:
    void paintEvent(QPaintEvent *event);        //重写绘制函数
    bool eventFilter(QObject *, QEvent *);      //过滤鼠标按下事件

private slots:
    void alertChange();           //执行报警改变操作
	void resetList();

private:
    QString defenceID;      //防区号
    DefenceType defenceType;//防区类型  00--07
    QString hostID;         //防区对应主机编号
    QString hostName;       //防区对应主机名称
    QString defenceSound;   //防区报警声音
    QString defenceImage;   //防区地图
    QString defenceUser;    //防区用户
    int defenceX;           //防区X坐标
    int defenceY;           //防区Y坐标
    DefenceStatus defenceStatus;//防区状态
	QString hostAddr_;
	int hostPort_;

    QRadialGradient *currentRG;
	QRadialGradient *labelOfNum;
    bool isChange;             //当前报警颜色是否为红色
	QTimer *myTimer;        //控制报警闪烁时钟

	QMap<QString,QString> warnningList_;//又一个映射，超级数组
	QMap<QString,QString> dangerList_;//
	QMutex mtx_;
	bool isAlert;

	void initForm();
};
#endif //DefenceButton_H
