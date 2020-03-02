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
	void on_btnOk_clicked();            //应用配置文件
	void on_btnClose_clicked();         //关闭界面
	void on_btnAdd_clicked();           //添加操作员
	void on_btnDelete_clicked();        //删除操作员
	void on_btnUpdate_clicked();        //修改操作员
private:
	Ui::systemSetting ui_;
	QStringList userName_;   //操作员姓名链表
	QStringList userInfo_;   //操作员信息链表

	void initForm();            //初始化界面
	void readUserInfo();        //加载操作员信息
	bool checkAdmin(QString UserName);                      //校验当前是否是管理员
	bool isExistUser(QString UserName);                     //校验操作员是否存在
	bool checkUserInfo(QString UserName,QString UserPwd);   //校验输入框是否为空
};

#endif
