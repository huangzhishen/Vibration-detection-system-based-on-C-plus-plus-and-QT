#ifndef CONFIG_H
#define CONFIG_H
#include <QString>


class Config{
public:
	static bool isMove;
	static QString AppPath;
	static int DeskWidth;
	static int DeskHeigth;
	static QString CurrentImage;
	static int timeGap;
	//���ڳ�ʼ��defenceID�ĳ�ʼ���б�
	static int defenceCount;

	static QString softwareTitle;

	//��¼��Ϣ
	static QString lastLoginUser;
	static QString currentUserName;
	static QString currentUserPassword;
	static QString currentUserType;
	
	//������Ϣ
	static int defenceSize;

	//����IP��PORT
	static int clientPort;
	static QString hostAddr;
	static int hostPort;

	
	
	//��ť�����߳�
	static int defenceBGWidth;
	static int defenceBGHeight;

	//��ťƫ��
	static int defenceOffsetX;
	static int defenceOffsetY;

	//��ʾ�߳�
	static int labelWidth;
	static int labelHeight;
	//��ʾƫ��
	static int labelOffsetX;
	static int labelOffsetY;

	static float warnZcr,warnAmp;
	static float dangerZcr,dangerAmp;
	
	static QString contactName;
	static QString contactTel;
	static QString contactMessage;
	static QString contactMessage1;


	static float zw1;
	static float zd1;
	static float ew1;
	static float ed1;
	static float zw2;
	static float zd2;
	static float ew2;
	static float ed2;
	static float zw3;
	static float zd3;
	static float ew3;
	static float ed3;
	static float zw4;
	static float zd4;
	static float ew4;
	static float ed4;
	static float zw5;
	static float zd5;
	static float ew5;
	static float ed5;
	static float zw6;
	static float zd6;
	static float ew6;
	static float ed6;
	static float zw7;
	static float zd7;
	static float ew7;
	static float ed7;
	static float zw8;
	static float zd8;
	static float ew8;
	static float ed8;


	static void ReadConfig();    //��ȡ�����ļ�,��main�����ʼ���س�������
//	static void ReadConfigchinese();
	static void WriteConfig();          //д�������ļ�,�ڸ��������ļ�������ر�ʱ����
	static bool isRecordAllData;
};
#endif