#ifndef USERLOGIN_H
#define USERLOGIN_H
#include <QtGui/QDialog>
#include "ui_userLogin.h"

class UserLogin: public QDialog
{
	Q_OBJECT
public:
	UserLogin(QDialog *parent = 0);
	~UserLogin();

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private slots:
	void on_btnLogin_clicked();
	void on_btnClose_clicked();
	void buttonClick();
	void on_btnKey_clicked();

private:
	Ui::userLogin ui_;
	QStringList userPwd_;
	QStringList userType_;
	void initForm();
};

#endif


