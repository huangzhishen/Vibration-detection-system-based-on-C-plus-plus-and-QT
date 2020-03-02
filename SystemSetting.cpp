#include "SystemSetting.h"
#include "Utility.h"
#include "Config.h"
#include <QSqlQuery>
#include <QDateTime>
#include <QSettings>

SystemSetting::SystemSetting(QDialog *parent)
	:QDialog(parent)
{
	ui_.setupUi(this);
	Utility::moveToCenter(this,Config::DeskWidth,Config::DeskHeigth);
	Utility::setFixSize(this);
	initForm();
	readUserInfo();

}

SystemSetting::~SystemSetting()
{

}


void SystemSetting::initForm()
{
	//是否按年或月备份
	bool BankYear=true;
	ui_.rbtnYear->setChecked(BankYear);
	ui_.rbtnMonth->setChecked(!BankYear);

	//加载年月日时分秒下拉框
	for (int i=1970;i<=2030;i++){
		ui_.cboxYear->addItem(QString::number(i));
	}

	for (int i=1;i<=12;i++){
		ui_.cboxMonth->addItem(QString::number(i));
	}

	for (int i=1;i<=31;i++){
		ui_.cboxDay->addItem(QString::number(i));
	}

	for (int i=0;i<24;i++){
		ui_.cboxHour->addItem(QString::number(i));
	}

	for (int i=0;i<60;i++){
		ui_.cboxMin->addItem(QString::number(i));
		ui_.cboxSec->addItem(QString::number(i));
	}
	//加载当前时间
	QStringList Now=QDateTime::currentDateTime().toString("yyyy-M-d-h-m-s").split("-");
	ui_.cboxYear->setCurrentIndex(ui_.cboxYear->findText(Now[0]));
	ui_.cboxMonth->setCurrentIndex(ui_.cboxMonth->findText(Now[1]));
	ui_.cboxDay->setCurrentIndex(ui_.cboxDay->findText(Now[2]));
	ui_.cboxHour->setCurrentIndex(ui_.cboxHour->findText(Now[3]));
	ui_.cboxMin->setCurrentIndex(ui_.cboxMin->findText(Now[4]));
	ui_.cboxSec->setCurrentIndex(ui_.cboxSec->findText(Now[5]));

	if (Config::currentUserType.compare(tr("管理员")) != 0)
	{
		ui_.btnSetDefault->setEnabled(false);
		ui_.btnAdd->setEnabled(false);
		ui_.btnDelete->setEnabled(false);
		ui_.btnUpdate->setEnabled(false);
		ui_.btnOk->setEnabled(false);
		
		//ui_.leWarnZcr->setEnabled(false);
        //ui_.leWarnAmp->setEnabled(false);
		//ui_.leDangerAmp->setEnabled(false);
	    //ui_.leDangerZcr->setEnabled(false);
	}

	//ui_.leWarnAmp->setText(QString("%1").arg(Config::warnAmp));
	//ui_.leWarnZcr->setText(QString("%1").arg(Config::warnZcr));
	//ui_.leDangerAmp->setText(QString("%1").arg(Config::dangerAmp));
	//ui_.leDangerZcr->setText(QString("%1").arg(Config::dangerZcr));

	
	   ui_.leName->setText(Config::contactName);
	   ui_.leTel->setText(tr("18716035785"));
	   ui_.leMsg->setText(Config::contactMessage);
	   ui_.leMsgg->setText(Config::contactMessage1);
	
	
}

void SystemSetting::readUserInfo()
{
	ui_.listUser->clear();
	userName_.clear();
	userInfo_.clear();
	QSqlQuery query;
	query.exec("select userName, userPassword, userType from userinfo");
	while(query.next()){
		//添加用户姓名到用户列表
		userName_ << query.value(0).toString();
		userInfo_ <<QString("%1|%2|%3")
			.arg(query.value(0).toString())
			.arg(query.value(1).toString())
			.arg(query.value(2).toString());
	}

	ui_.listUser->addItems(userName_);
	if (userName_.count() > 0) {
		ui_.listUser->setCurrentRow(0);
	}
}

bool SystemSetting::checkAdmin(QString UserName)
{
	if (UserName==""){
		Utility::ShowMessageBoxError("请选择要删除或者修改的操作员!");
		return false;
	}

	if (UserName=="admin"){
		Utility::ShowMessageBoxError("超级管理员不能删除和修改!");
		return false;
	}
	return true;
}

bool SystemSetting::isExistUser(QString UserName)
{
	foreach (QString name, userName_){
		if (name == UserName){
			Utility::ShowMessageBoxError("该用户已经存在,请重新填写!");
			ui_.txtUserName->setFocus();
			return true;
		}
	}
	return false;
}

bool SystemSetting::checkUserInfo(QString UserName,QString UserPwd)
{
	if (UserName == ""){
		Utility::ShowMessageBoxError("用户姓名不能为空,请重新填写!");
		ui_.txtUserName->setFocus();
		return false;
	}

	if (UserPwd == ""){
		Utility::ShowMessageBoxError("用户密码不能为空,请重新填写!");
		ui_.txtUserPwd->setFocus();
		return false;
	}
	return true;
}

void  SystemSetting::on_listUser_currentRowChanged(int currentRow)
{
	if (currentRow < 0)
	{
		return;
	}
	QStringList user=userInfo_[currentRow].split("|");
	ui_.txtUserName->setText(user[0]);
	ui_.txtUserPwd->setText(user[1]);
	ui_.txtUserType->setCurrentIndex(ui_.txtUserType->findText(user[2]));
}

void SystemSetting::on_btnOk_clicked()
{
	QString fileName = Config::AppPath + "Config.ini";
	QSettings configFile(fileName, QSettings::IniFormat);

	configFile.beginGroup("AppConfig");
	configFile.setValue("contactingName",ui_.leName->text().toUtf8());
	configFile.setValue("contactingTel",ui_.leTel->text().toUtf8());
	configFile.setValue("contactingMessage",ui_.leMsg->text().toUtf8());
	configFile.setValue("contactingMessage1",ui_.leMsgg->text().toUtf8());
	configFile.endGroup();

	this->readUserInfo();//重新加载用户列表
	done(1);//关闭界面返回
	this->close();
}

void SystemSetting::on_btnClose_clicked()
{
	done(0);//关闭界面返回
	this->close();
}
void SystemSetting::on_btnAdd_clicked()
{
	QString UserName=ui_.txtUserName->text();
	QString UserPwd=ui_.txtUserPwd->text();
	QString UserType=ui_.txtUserType->currentText();

	if (!this->checkUserInfo(UserName,UserPwd)){return;}
	if (this->isExistUser(UserName)){return;}

	QSqlQuery query;
	QString sql="insert into userinfo ( userName, userPassword , userType )";
	sql+="values('"+UserName+"','";
	sql+=UserPwd+"','";
	sql+=UserType+"')";
	query.exec(sql);
	this->readUserInfo();//重新加载用户列表
}
void SystemSetting::on_btnDelete_clicked()
{
	QString TempUserName=ui_.listUser->currentIndex().data().toString();
	if (!this->checkAdmin(TempUserName)){return;}

	if (Utility::ShowMessageBoxQuesion("Do you really want to delete user?")==0)
	{
		QSqlQuery query;
		QString sql="delete from userInfo where userName='"+TempUserName+"'";
		query.exec(sql);
		this->readUserInfo();//重新加载用户列表
	}
}
void SystemSetting::on_btnUpdate_clicked()
{

}