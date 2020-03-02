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

    explicit DefenceButton(QWidget *parent=0);//explicit�ؼ���

    explicit DefenceButton(QWidget *parent, QString defenceID, DefenceType defenceType, QString hostID, QString hostName,  
		QString defenceSound,QString defenceImage, QString defenceUser, int defenceX, int defenceY, DefenceStatus defenceStatus);


    void SetDefenceID(QString defenceID);                       //���÷���ID
    void SetDefenceStatus(DefenceStatus defenceStatus);         //���÷���״̬,�������������������·��
	void setHostAddrAndPort(QString hostAddr,int hostPort);
	QString getHostAddr();
	int getHostPort();
    QString GetDefenceID()const{return defenceID;}              //��ȡ����ID
    DefenceType GetDefenceType()const{return defenceType;}      //��ȡ��������
    QString GetHostID()const{return hostID;}                    //��ȡ������Ӧ����ID
    QString GetHostName()const{return hostName;}                //��ȡ������Ӧ��������

    QString GetDefenceSound()const{return defenceSound;}        //��ȡ������������
    QString GetDefenceImage()const {return defenceImage;}       //��ȡ������Ӧ��ͼ�ļ�����
    QString GetDefenceUser()const{return defenceUser;}          //��ȡ�����û�
    int GetDefenceX()const{return defenceX;}                    //��ȡ����X����
    int GetDefenceY()const{return defenceY;}                    //��ȡ����Y����
    DefenceStatus GetDefenceStatus()const{return defenceStatus;}//��ȡ����״̬

	void appendWarnList(QString defenceID,QString currTime);
	void appendDangerList(QString defenceID,QString currTime);

	int getWarningCount() { return warnningList_.size(); }
	int getDangerCount() { return dangerList_.size(); }

signals:
	void sig_resetList(int ID);
protected:
    void paintEvent(QPaintEvent *event);        //��д���ƺ���
    bool eventFilter(QObject *, QEvent *);      //������갴���¼�

private slots:
    void alertChange();           //ִ�б����ı����
	void resetList();

private:
    QString defenceID;      //������
    DefenceType defenceType;//��������  00--07
    QString hostID;         //������Ӧ�������
    QString hostName;       //������Ӧ��������
    QString defenceSound;   //������������
    QString defenceImage;   //������ͼ
    QString defenceUser;    //�����û�
    int defenceX;           //����X����
    int defenceY;           //����Y����
    DefenceStatus defenceStatus;//����״̬
	QString hostAddr_;
	int hostPort_;

    QRadialGradient *currentRG;
	QRadialGradient *labelOfNum;
    bool isChange;             //��ǰ������ɫ�Ƿ�Ϊ��ɫ
	QTimer *myTimer;        //���Ʊ�����˸ʱ��

	QMap<QString,QString> warnningList_;//��һ��ӳ�䣬��������
	QMap<QString,QString> dangerList_;//
	QMutex mtx_;
	bool isAlert;

	void initForm();
};
#endif //DefenceButton_H
