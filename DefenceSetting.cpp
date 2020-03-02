#include "DefenceSetting.h"
#include "Defencebutton.h"
#include "Config.h"
#include "Utility.h"
#include <QDir>
#include <QEvent>
#include <QKeyEvent>
#include <QSqlQuery>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QSettings>

DefenceSetting::DefenceSetting(QDialog *parent)
	:QDialog(parent)
{
	ui_.setupUi(this);
	Utility::moveToCenter(this,Config::DeskWidth,Config::DeskHeigth);
	Utility::setFixSize(this);
	initForm();
	getImageFile();
	getSoundFile();
	readHostInfo();
	initDefenceArea();
	readDefenceInfo();
	setDefenceButtonVisible();
	isAddOrUpdateDefence_ = false; 
	btnTemp_ = NULL;
	ui_.gboxMain->installEventFilter(this);
	
}


DefenceSetting::~DefenceSetting()
{
}

void DefenceSetting::initForm()
{
	for (int i=1;i<=8;i++){
		if (i<10){
			ui_.txtDefenceID->addItem(QString("00%1").arg(i));
			continue;
		}
		if (i<100){
			ui_.txtDefenceID->addItem(QString("0%1").arg(i));
			continue;
		}
	}
	//填充防区类型
	QStringList DefenceType;
	DefenceType.append(tr("1"));
	DefenceType.append(tr("2"));
	DefenceType.append(tr("3"));
	DefenceType.append(tr("4"));
	DefenceType.append(tr("5"));

	ui_.txtDefenceType->addItems(DefenceType);
	ui_.txtDefenceType->setCurrentIndex(0);

	ui_.treeDefence->setColumnCount(1); //设置列数
	ui_.treeDefence->setColumnWidth(0,200);
	ui_.treeDefence->setEditTriggers(QAbstractItemView::NoEditTriggers);	//不能编辑

	/*if (Config::currentUserType.compare(tr("管理员")) != 0)
	{
	ui_.btnAdd->setEnabled(false);
	ui_.btnDelete->setEnabled(false);
	ui_.btnModifty->setEnabled(false);
	ui_.btnMove->setEnabled(false);
	ui_.btnImportMap->setEnabled(false);
	}*/
}

void DefenceSetting::getImageFile()
{
	ui_.txtDefenceImage->clear();
	QDir imagePath(Config::AppPath+"MAP");
	QStringList filter;
	filter<<"*.jpg"<<"*.bmp"<<"*.png";
	ui_.txtDefenceImage->addItems(imagePath.entryList(filter));
}

void DefenceSetting::getSoundFile()
{
	ui_.txtDefenceSound->clear();
	QDir soundPath(Config::AppPath + "SOUND");
	QStringList filter;
	filter << "*.wav";
	ui_.txtDefenceSound->addItems(soundPath.entryList(filter));
}
void DefenceSetting::readHostInfo()
{
	hostID_.clear();
	hostName_.clear();
	ui_.txtHostID->clear();
	ui_.txtHostName->clear();

	QSqlQuery query;
	QString sql = "select hostID, hostName from hostInfo";
	query.exec(sql);
	while(query.next()){
		hostID_ << query.value(0).toString();
		hostName_ << query.value(1).toString();
	}
	ui_.txtHostID->addItems(hostID_);
	ui_.txtHostName->addItems(hostName_);
}

void DefenceSetting::readDefenceInfo()
{
	ui_.treeDefence->setColumnWidth(0,200);
	defenceIDHostID_.clear();
	ui_.treeDefence->clear();
	QSqlQuery query;
	QString sql = "select hostID, hostName from hostInfo";
	query.exec(sql);
	while (query.next())
	{
		QString hostID = query.value(0).toString();
		QString hostName = query.value(1).toString();
		QTreeWidgetItem *item = new QTreeWidgetItem(ui_.treeDefence,QStringList("["+hostID+"]" + hostName));
		item->setIcon(0,QIcon(":/image/tree1.png"));

		QSqlQuery defenceQuery;
		QString defencesql = "select defenceID from defenceInfo where hostID='" + hostID + "'";
		defenceQuery.exec(defencesql);
		while(defenceQuery.next()){
			QString defenceID = defenceQuery.value(0).toString();
			defenceIDHostID_ << QString("%1|%2").arg(defenceID).arg	(hostID	);

			QTreeWidgetItem *itemTree = new QTreeWidgetItem(item,QStringList("Area." + defenceID));
			itemTree->setIcon(0,QIcon(":/image/tree2.png"));
			item->addChild(itemTree);
		}
	}
	ui_.treeDefence->expandAll();
}


void DefenceSetting::initDefenceArea()
{
	qDeleteAll(ui_.gboxMain->findChildren<DefenceButton*>());
	QSqlQuery query;
	QString sql = "select * from defenceinfo";
	query.exec(sql);
	while (query.next())
	{
		QString defenceID = query.value(0).toString();
		int defenceType = query.value(1).toInt();
		QString hostID = query.value(2).toString();
		QString hostName = query.value(3).toString();
		QString defenceMap = query.value(4).toString();
		QString defenceSound = query.value(5).toString();
		int defencePosX = query.value(6).toFloat() * ui_.gboxMain->width();
		int defencePosY = query.value(7).toFloat() * ui_.gboxMain->height();

		DefenceButton *btn = new DefenceButton(ui_.gboxMain, defenceID, Utility::getDefenceType(defenceType), hostID, hostName,											defenceSound,defenceMap,Config::currentUserName, defencePosX, defencePosY, DefenceStatus_init);
		btn->setGeometry(defencePosX,defencePosY, Config::defenceSize,Config::defenceSize);
	}
	
}

bool DefenceSetting::eventFilter(QObject *obj, QEvent *event)
{
	if (obj==ui_.gboxMain && isAddOrUpdateDefence_){
		if (event->type()==QEvent::MouseButtonPress) {
			QMouseEvent *mouseEvent=static_cast<QMouseEvent *>(event);
			if (mouseEvent->button()==Qt::LeftButton) {
				btnTemp_->setVisible(true);
				btnTemp_->move(mouseEvent->x(),mouseEvent->y());
				ui_.txtDefenceX->setText(QString::number(mouseEvent->x()));
				ui_.txtDefenceY->setText(QString::number(mouseEvent->y()));
				return true;
			}
		}
	}
	return QObject::eventFilter(obj,event);
}

void DefenceSetting::on_btnAdd_clicked()
{
	if (ui_.btnAdd->text()==tr("添加(&A)")){
		ui_.btnDelete->setEnabled(false);
		ui_.btnModifty->setEnabled(false);
		ui_.btnImportMap->setEnabled(false);
		ui_.treeDefence->setEnabled(false);

		ui_.txtDefenceX->clear();
		ui_.txtDefenceY->clear();
		ui_.btnAdd->setText(tr("保存(&S)"));
		isAddOrUpdateDefence_=true;

		btnTemp_=new DefenceButton(ui_.gboxMain);
		btnTemp_->setGeometry(0, 0,Config::defenceSize, Config::defenceSize);
		btnTemp_->SetDefenceStatus(DefenceStatus_init);
		btnTemp_->setVisible(true);
		QList<DefenceButton *> btn=ui_.gboxMain->findChildren<DefenceButton *>();
		btn.size();
	}
	else{
		QString defenceID=ui_.txtDefenceID->currentText();
		QString defenceType=ui_.txtDefenceType->currentText();
		QString hostID=ui_.txtHostID->currentText();
		QString hostName=ui_.txtHostName->currentText();
		QString defenceImage=ui_.txtDefenceImage->currentText();
		QString defenceSound = ui_.txtDefenceSound->currentText();
		QString defenceX=ui_.txtDefenceX->text();
		QString defenceY=ui_.txtDefenceY->text();
		float posX = defenceX.toFloat() / ui_.gboxMain->width();
		float posY = defenceY.toFloat() / ui_.gboxMain->height();

		if (!checkDefenceInfo(hostID,hostName,defenceImage,defenceX,defenceY))
		{
			return ;
		}
		if(isExistDefence(defenceID,hostID)){
			return ;
		}
		
		QSqlQuery query;
		QString sql = "insert into defenceInfo (defenceID, defenceType, hostID, hostName, map, music, posX, posY) values('";
		sql += defenceID + "','";
		sql += defenceType + "','";
		sql += hostID + "','";
		sql += hostName + "','";
		sql += defenceImage + "','";
		sql += defenceSound + "','";
		sql += QString::number(posX,'f',4) + "','";
		sql += QString::number(posY,'f',4) + "')";
		query.exec(sql);

		ui_.btnDelete->setEnabled(true);
		ui_.btnModifty->setEnabled(true);
		ui_.btnImportMap->setEnabled(true);
		ui_.treeDefence->setEnabled(true);
		ui_.btnAdd->setText(tr("添加(&A)"));
		isAddOrUpdateDefence_ = false;

		initDefenceArea();
		readDefenceInfo();
		setDefenceButtonVisible();
	}
}
void DefenceSetting::on_btnDelete_clicked()
{
	QString tmp = ui_.treeDefence->currentIndex().data().toString();
	if (tmp == "")
	{
		Utility::ShowMessageBoxError("请选择需要删除的防区!");
		return ;
	}
	QString defenceID = ui_.txtDefenceID->currentText();
	QString hostID = ui_.txtHostID->currentText();

	if (Utility::ShowMessageBoxQuesion("确定要删除防区吗?") == 0) 
	{
		QSqlQuery query;
		QString sql = "delete from defenceInfo where defenceID='";
		sql += defenceID + "' and hostID='";
		sql += hostID + "'";
		query.exec(sql);
		initDefenceArea();
		readDefenceInfo();
		setDefenceButtonVisible();
	}


}
void DefenceSetting::on_btnImportMap_clicked()
{
	QString fileName = Utility::getFileName(tr("选择文件(*.jpg *.png *.bmp)"));
	if (fileName == "")
	{
		return ;
	}
	bool ok = Utility::copyFile(fileName,Config::AppPath + "\\" + Utility::getFileNameWithExt(fileName));
	if (ok){
		getImageFile();
		Utility::ShowMessageBoxInfo("导入图片成功!");
	}
	else{
		Utility::ShowMessageBoxInfo("导入图片失败!");
	}
	

}


void DefenceSetting::on_btnModifty_clicked()//防区设置中的修改按钮
{
	QString tmp = ui_.treeDefence->currentIndex().data().toString();
	if (tmp == "")
	{
		Utility::ShowMessageBoxError("请选择需要修改的防区!");
		return ;
	}
	
	if (ui_.btnModifty->text() == tr("修改(&U)"))//这里的逻辑是：先判断修改按钮是否点击，没有则可以继续修改，此按钮变成保存。
	{
		ui_.btnAdd->setEnabled(false);
		ui_.btnDelete->setEnabled(false);
		ui_.btnImportMap->setEnabled(false);
		ui_.btnModifty->setText(tr("保存(&S)"));
		isAddOrUpdateDefence_ = true;

		btnTemp_ = new DefenceButton(ui_.gboxMain);
		btnTemp_->setGeometry(0,0,Config::defenceSize,Config::defenceSize);
		btnTemp_->SetDefenceStatus(DefenceStatus_init);
		btnTemp_->setVisible(true);

		tempDefenceID_ = ui_.txtDefenceID->currentText();
		tempHostID_ = ui_.txtHostID->currentText();
	}
	else//如果此按钮已经被点击，则下面的操作就是保存对话框的值
	{
		QString defenceID=ui_.txtDefenceID->currentText();
		QString defenceType=ui_.txtDefenceType->currentText();
		QString hostID=ui_.txtHostID->currentText();
		QString hostName=ui_.txtHostName->currentText();
		QString defenceImage=ui_.txtDefenceImage->currentText();
		QString defenceSound = ui_.txtDefenceSound->currentText();
		QString defenceX=ui_.txtDefenceX->text();
		QString defenceY=ui_.txtDefenceY->text();
		float posX  = defenceX.toFloat() / ui_.gboxMain->width();
		float posY = defenceY.toFloat() / ui_.gboxMain->height();

		QString fileName = Config::AppPath + "Config.ini";
		QSettings configFile(fileName, QSettings::IniFormat);

		switch(defenceID.toInt())
		{
		case 1:
			configFile.beginGroup("AppConfig");
			configFile.setValue("zw1",ui_.ZW->text().toFloat());
			configFile.setValue("zd1",ui_.ZD->text().toFloat());  
			configFile.setValue("ew1",ui_.EW->text().toFloat());
			configFile.setValue("ed1",ui_.ED->text().toFloat());  
	        configFile.endGroup();
			break;
		case 2:
			configFile.beginGroup("AppConfig");
			configFile.setValue("zw2",ui_.ZW->text().toFloat());
			configFile.setValue("zd2",ui_.ZD->text().toFloat());  
			configFile.setValue("ew2",ui_.EW->text().toFloat());
			configFile.setValue("ed2",ui_.ED->text().toFloat());  
			configFile.endGroup();  
			break;
		case 3:
			configFile.beginGroup("AppConfig");
			configFile.setValue("zw3",ui_.ZW->text().toFloat());
			configFile.setValue("zd3",ui_.ZD->text().toFloat());  
			configFile.setValue("ew3",ui_.EW->text().toFloat());
			configFile.setValue("ed3",ui_.ED->text().toFloat());  
			configFile.endGroup();
			break;
		case 4:
			configFile.beginGroup("AppConfig");
			configFile.setValue("zw4",ui_.ZW->text().toFloat());
			configFile.setValue("zd4",ui_.ZD->text().toFloat());  
			configFile.setValue("ew4",ui_.EW->text().toFloat());
			configFile.setValue("ed4",ui_.ED->text().toFloat());  
			configFile.endGroup();
			break;
		case 5:
			configFile.beginGroup("AppConfig");
			configFile.setValue("zw5",ui_.ZW->text().toFloat());
			configFile.setValue("zd5",ui_.ZD->text().toFloat());  
			configFile.setValue("ew5",ui_.EW->text().toFloat());
			configFile.setValue("ed5",ui_.ED->text().toFloat());  
			configFile.endGroup();
			break;
		case 6:
			configFile.beginGroup("AppConfig");
			configFile.setValue("zw6",ui_.ZW->text().toFloat());
			configFile.setValue("zd6",ui_.ZD->text().toFloat());  
			configFile.setValue("ew6",ui_.EW->text().toFloat());
			configFile.setValue("ed6",ui_.ED->text().toFloat());  
			configFile.endGroup();
			break;
		case 7:
			configFile.beginGroup("AppConfig");
			configFile.setValue("zw7",ui_.ZW->text().toFloat());
			configFile.setValue("zd7",ui_.ZD->text().toFloat());  
			configFile.setValue("ew7",ui_.EW->text().toFloat());
			configFile.setValue("ed7",ui_.ED->text().toFloat());  
			configFile.endGroup();
			break;
		case 8:
			configFile.beginGroup("AppConfig");
			configFile.setValue("zw8",ui_.ZW->text().toFloat());
			configFile.setValue("zd8",ui_.ZD->text().toFloat());  
			configFile.setValue("ew8",ui_.EW->text().toFloat());
			configFile.setValue("ed8",ui_.ED->text().toFloat());  
			configFile.endGroup();
			break;
		}
	
		if (!checkDefenceInfo(hostID,hostName,defenceImage,defenceX,defenceY))
		{
			return ;
		}
		QSqlQuery query;
		QString sql="update defenceInfo set";
		sql+=" defenceID='"+defenceID;
		sql+="',defenceType='"+defenceType;
		sql+="',hostID='"+hostID;
		sql+="',hostName='"+hostName;
		sql+="',music='"+defenceSound;
		sql+="',map='"+defenceImage;
		sql+="',posX='"+QString::number(posX,'f',4);
		sql+="',posY='"+QString::number(posY,'f',4);
		sql+="' where defenceID='"+tempDefenceID_;
		sql+="' and hostID='"+tempHostID_+"'";
		query.exec(sql);

		ui_.btnDelete->setEnabled(true);
		ui_.btnAdd->setEnabled(true);
		ui_.btnImportMap->setEnabled(true);
		ui_.treeDefence->setEnabled(true);
		ui_.btnModifty->setText(tr("修改(&U)"));
		isAddOrUpdateDefence_ = false;

		initDefenceArea();
		readDefenceInfo();
		setDefenceButtonVisible();
	}
}




void DefenceSetting::setDefenceButtonVisible()
{
	QList<DefenceButton *> btn=ui_.gboxMain->findChildren<DefenceButton *>();
	foreach (DefenceButton *b, btn){
		if (b->GetDefenceImage()==Config::CurrentImage){
			b->setVisible(true);
		}else{
			b->setVisible(false);
		}
	}
}

void DefenceSetting::on_txtDefenceImage_currentIndexChanged(const QString &arg1)
{
	Config::CurrentImage=arg1;
	ui_.gboxMain->setStyleSheet(QString("border-image:url(%1MAP/%2);")
		.arg(Config::AppPath)
		.arg(Config::CurrentImage));
	setDefenceButtonVisible();
}
//
void DefenceSetting::on_treeDefence_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current == 0 || current->parent() == 0)
	{
		return ;
	}
	QString hostID = current->parent()->text(0).split("[")[1].split("]")[0];
	QString defenceID = current->text(0).split(".")[1];
	QSqlQuery query;
	QString sql = "select * from defenceInfo where defenceID='" + defenceID + "' and hostID='" + hostID + "'";
	query.exec(sql);
	while (query.next())
	{
		QString defenceType = query.value(1).toString();
		QString hostName = query.value(3).toString();
		QString defenceMap = query.value(4).toString();
		QString defenceSound = query.value(5).toString();
		QString defenceX = query.value(6).toString();
		QString defenceY = query.value(7).toString();
		int posX = defenceX.toFloat() * ui_.gboxMain->width();
		int posY = defenceY.toFloat() * ui_.gboxMain->height();
		defenceX = QString("%1").arg(posX);
		defenceY = QString("%1").arg(posY);

		ui_.txtDefenceID->setCurrentIndex(ui_.txtDefenceID->findText(defenceID));
		ui_.txtDefenceType->setCurrentIndex(ui_.txtDefenceType->findText(defenceType));
		ui_.txtHostID->setCurrentIndex(ui_.txtHostID->findText(hostID));
		ui_.txtDefenceSound->setCurrentIndex(ui_.txtDefenceSound->findText(defenceSound));
		ui_.txtDefenceImage->setCurrentIndex(ui_.txtDefenceImage->findText(defenceMap));
		ui_.txtDefenceX->setText(defenceX);
		ui_.txtDefenceY->setText(defenceY);

		switch (defenceID.toInt())
		{
		case 1:
			ui_.ED->setText(QString("%1").arg(Config::ed1));
			ui_.EW->setText(QString("%1").arg(Config::ew1));
			ui_.ZD->setText(QString("%1").arg(Config::zd1));
			ui_.ZW->setText(QString("%1").arg(Config::zw1));
			break;
		case 2:
			ui_.ED->setText(QString("%1").arg(Config::ed2));
			ui_.EW->setText(QString("%1").arg(Config::ew2));
			ui_.ZD->setText(QString("%1").arg(Config::zd2));
			ui_.ZW->setText(QString("%1").arg(Config::zw2));
			break;
		case 3:
			ui_.ED->setText(QString("%1").arg(Config::ed3));
			ui_.EW->setText(QString("%1").arg(Config::ew3));
			ui_.ZD->setText(QString("%1").arg(Config::zd3));
			ui_.ZW->setText(QString("%1").arg(Config::zw3));
			break;
		case 4:
			ui_.ED->setText(QString("%1").arg(Config::ed4));
			ui_.EW->setText(QString("%1").arg(Config::ew4));
			ui_.ZD->setText(QString("%1").arg(Config::zd4));
			ui_.ZW->setText(QString("%1").arg(Config::zw4));
			break;
		case 5:
			ui_.ED->setText(QString("%1").arg(Config::ed5));
			ui_.EW->setText(QString("%1").arg(Config::ew5));
			ui_.ZD->setText(QString("%1").arg(Config::zd5));
			ui_.ZW->setText(QString("%1").arg(Config::zw5));
			break;
		case 6:
			ui_.ED->setText(QString("%1").arg(Config::ed6));
			ui_.EW->setText(QString("%1").arg(Config::ew6));
			ui_.ZD->setText(QString("%1").arg(Config::zd6));
			ui_.ZW->setText(QString("%1").arg(Config::zw6));
			break;
		case 7:
			ui_.ED->setText(QString("%1").arg(Config::ed7));
			ui_.EW->setText(QString("%1").arg(Config::ew7));
			ui_.ZD->setText(QString("%1").arg(Config::zd7));
			ui_.ZW->setText(QString("%1").arg(Config::zw7));
			break;
		case 8:
			ui_.ED->setText(QString("%1").arg(Config::ed8));
			ui_.EW->setText(QString("%1").arg(Config::ew8));
			ui_.ZD->setText(QString("%1").arg(Config::zd8));
			ui_.ZW->setText(QString("%1").arg(Config::zw8));
			break;
		}
		
		QList<DefenceButton *> btn=ui_.gboxMain->findChildren<DefenceButton *>();
		foreach (DefenceButton *b, btn)
		{
			if (b->GetDefenceID()==defenceID && b->GetHostID()==hostID)
			{
				b->SetDefenceStatus(DefenceStatus_select);
			}
			else
			{
				b->SetDefenceStatus(DefenceStatus_init);
			}
		}
	}
}

bool DefenceSetting::checkDefenceInfo(QString hostID, QString hostName, QString defenceImage, QString posX, QString posY)
{
	if (hostID == "")
	{
		Utility::ShowMessageBoxError("");
		ui_.txtHostID->setFocus();
		return false;
	}
	if (hostName == "")
	{
		Utility::ShowMessageBoxError("");
		ui_.txtHostName->setFocus();
		return false;
	}
	if (defenceImage == "")
	{
		Utility::ShowMessageBoxError("");
		ui_.txtDefenceImage->setFocus();
		return false;
	}

	if (posX == "" || posY == "")
	{
		Utility::ShowMessageBoxError("");
		ui_.txtDefenceX->setFocus();
		return false;
	}
	return true;
}

bool DefenceSetting::isExistDefence(QString defenceID, QString hostID)//添加防区是判断是否已经添加
{
	QString temp = QString("%1|%2").arg(defenceID).arg(hostID);
	foreach(QString str , defenceIDHostID_){
		if (temp == str)
		{
			Utility::ShowMessageBoxError("已经存在该防区");
			return true;
		}
	}
	return false;
}