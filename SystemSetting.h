#ifndef SYSTEMSETTING_H
#define SYSTEMSETTING_H
#include <QtGui/QDialog>
#include "ui_systemSetting.h"

class SystemSetting:public QDialog
{
	Q_OBJECT
public:
	SystemSetting(QDialog *parent = 0);
	~SystemSetting();
private slots:
	void on_listUser_currentRowChanged(int currentRow);
	void on_btnOk_clicked();            //Ӧ�������ļ�
	void on_btnClose_clicked();         //�رս���
	void on_btnAdd_clicked();           //��Ӳ���Ա
	void on_btnDelete_clicked();        //ɾ������Ա
	void on_btnUpdate_clicked();        //�޸Ĳ���Ա
private:
	Ui::systemSetting ui_;
	QStringList userName_;   //����Ա��������
	QStringList userInfo_;   //����Ա��Ϣ����

	void initForm();            //��ʼ������
	void readUserInfo();        //���ز���Ա��Ϣ
	bool checkAdmin(QString UserName);                      //У�鵱ǰ�Ƿ��ǹ���Ա
	bool isExistUser(QString UserName);                     //У�����Ա�Ƿ����
	bool checkUserInfo(QString UserName,QString UserPwd);   //У��������Ƿ�Ϊ��
};

#endif
