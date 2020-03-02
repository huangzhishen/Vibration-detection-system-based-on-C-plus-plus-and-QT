#include "HostSetting.h"
#include "Config.h"
#include "Utility.h"
#include <QSqlQuery>

HostSetting::HostSetting(QDialog *parent)
	:QDialog(parent)
{
	ui_.setupUi(this);
	Utility::moveToCenter(this,Config::DeskWidth,Config::DeskHeigth);
	Utility::setFixSize(this);
	initForm();
	readHostInfo();
}


HostSetting::~HostSetting()
{
}


void HostSetting::initForm()
{
	if(Config::currentUserType.compare(tr("����Ա")) != 0){
		ui_.btnAdd->setEnabled(false);
		ui_.btnDelete->setEnabled(false);
		ui_.btnUpdate->setEnabled(false);
	}
}

void HostSetting::readHostInfo()
{
	ui_.listHost->clear();
	hostInfo_.clear();

	QSqlQuery query;
	QString sql = "select hostID, hostName, hostIP, hostPort from hostInfo";
	query.exec(sql);
	while (query.next())
	{
		QString hostInfo = QString("%1|%2|%3|%4")
							.arg(query.value(0).toString())
							.arg(query.value(1).toString())
							.arg(query.value(2).toString())
							.arg(query.value(3).toString());
		hostInfo_ << hostInfo;
		ui_.listHost->addItem("["+query.value(0).toString() + "]"+query.value(1).toString());
	}
}

bool HostSetting::checkHostInfo(QString hostID, QString hostName, QString hostIP, QString hostPort)
{
	if (hostID == "")
	{
		Utility::ShowMessageBoxError("������Ų���Ϊ�գ�����������!");
		ui_.txtHostID->setFocus();
		return false;
	}

	if (hostName == "")
	{
		Utility::ShowMessageBoxError("�������Ʋ���Ϊ�գ�����������!");
		ui_.txtHostName->setFocus();
		return false;
	}

	if (hostIP == "")
	{
		Utility::ShowMessageBoxError("����IP����Ϊ�գ�����������!");
		ui_.txtHostConnectValue1->setFocus();
		return false;
	}

	if (hostPort == "")
	{
		Utility::ShowMessageBoxError("�����˿ڲ���Ϊ�գ�����������!");
		ui_.txtHostConnectValue2->setFocus();
		return false;
	}

	return true;
}


void HostSetting::on_listHost_currentRowChanged(int currentRow)
{
	if (currentRow < 0)
	{
		return;
	}
	QStringList hostInfo = hostInfo_[currentRow].split("|");
	ui_.txtHostID->setText(hostInfo[0]);
	ui_.txtHostName->setText(hostInfo[1]);
	ui_.txtHostConnectValue1->setText(hostInfo[2]);
	ui_.txtHostConnectValue2->setText(hostInfo[3]);
	
}
void HostSetting::on_btnAdd_clicked()
{
	QString hostID = ui_.txtHostID->text();
	QString hostName = ui_.txtHostName->text();
	QString hostIP = ui_.txtHostConnectValue1->text();
	QString hostPort = ui_.txtHostConnectValue2->text();

	if (!checkHostInfo(hostID, hostName, hostIP, hostPort))
	{
		return;
	}
	if (Utility::isHostExist(hostID))
	{
		Utility::ShowMessageBoxError("����������Ѿ����ڣ����������룡");
		ui_.txtHostID->setFocus();
		return ;
	}

	QSqlQuery query;
	QString sql = "insert into hostInfo (hostID, hostName, hostIP, hostPort) values ('";
	sql += hostID + "','";
	sql += hostName + "','";
	sql += hostIP + "','";
	sql += hostPort +"')";
	query.exec(sql);

	readHostInfo();
}
void HostSetting::on_btnDelete_clicked()
{
	QString hostInfo = ui_.listHost->currentIndex().data().toString();
	if (hostInfo == "")
	{
		Utility::ShowMessageBoxError("��ѡ��Ҫɾ����������");
		return ;
	}
	if (Utility::ShowMessageBoxQuesion("ȷ��Ҫɾ�������𣿶�Ӧ������һ��ɾ����") == 0)
	{
		ui_.txtHostID->clear();
		ui_.txtHostName->clear();
		ui_.txtHostConnectValue1->clear();
		ui_.txtHostConnectValue2->clear();

		QString hostID = hostInfo.split("[")[1].split("]")[0];
		QSqlQuery query;
		QString sql = "delete from hostInfo where hostID='" + hostID + "'";
		query.exec(sql);
		readHostInfo();
	}

	return ;
}
void HostSetting::on_btnUpdate_clicked()
{
	QString hostInfo = ui_.listHost->currentIndex().data().toString();
	if (hostInfo == "")
	{
		Utility::ShowMessageBoxError("��ѡ��Ҫ�޸ĵ�����!");
		return ;
	}

	QString hostID = ui_.txtHostID->text();
	QString hostName = ui_.txtHostName->text();
	QString hostIP = ui_.txtHostConnectValue1->text();
	QString hostPort = ui_.txtHostConnectValue2->text();

	if (!checkHostInfo(hostID, hostName, hostIP, hostPort))
	{
		return ;
	}

	QString oldHostID = hostInfo.split("[")[1].split("]")[0];
	QSqlQuery query;
	QString sql = "update hostInfo set ";
	sql += "hostID='" +hostID + "',";
	sql += "hostName='" + hostName + "',";
	sql += "hostIP='" + hostIP + "',";
	sql += "hostPort='" + hostPort + "' ";
	sql	+= "where hostId='" + oldHostID + "'";
	query.exec(sql);

	readHostInfo();
	return ;
}