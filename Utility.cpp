#include "Utility.h"
#include "Config.h"
#include <QFile>
#include <QSqlQuery>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QDateTime>
bool Utility::isMainWidget = false;
QMutex Utility::reqMtx;
bool Utility::isRequesting = false;

void Utility::ShowMessageBoxError(const char* info)  /*显示错误窗口*/
{
	QMessageBox msg;
	msg.setStyleSheet("font: 12pt '宋体';");
	msg.setWindowTitle(tr("错误"));
	msg.setText(tr(info));
	msg.setIcon(QMessageBox::Critical);
	msg.setCursor(QCursor(Qt::PointingHandCursor));
	msg.addButton(tr("确定"),QMessageBox::ActionRole);//目测第二个参数的意思是表示该按钮的功能
	msg.exec();
}   

int Utility::ShowMessageBoxQuesion(const char* info)      /*显示询问窗口*/
{
	QMessageBox msg;
	msg.setStyleSheet("font: 12pt '宋体';");
	msg.setWindowTitle(tr("询问"));
	msg.setText(tr(info));
	msg.setIcon(QMessageBox::Question);
	msg.setCursor(QCursor(Qt::PointingHandCursor));
	msg.addButton(tr("确定"),QMessageBox::ActionRole);
	msg.addButton(tr("取消"),QMessageBox::RejectRole);

	return msg.exec();
} 

void Utility::ShowMessageBoxInfo(const char* info)
{
	QMessageBox msg;
	msg.setStyleSheet("font: 12pt '宋体';");
	msg.setWindowTitle(tr("提示"));
	msg.setText(tr(info));
	msg.setIcon(QMessageBox::Information);
	msg.setCursor(QCursor(Qt::PointingHandCursor));
	msg.addButton(tr("确定"),QMessageBox::ActionRole);
	msg.exec();
}    

bool Utility::isFileExist(QString &fileName)
{
	QFile file(fileName);//
	return file.exists();
}

bool Utility::isHostExist(QString &hostID)
{
	QSqlQuery query;
	QString sql = "select hostID from `hostInfo` where hostID='" + hostID + "'";
	query.exec(sql);
	return query.next();
}

//准确来说，不是文件名，而是文件路径
QString Utility::getFileName(QString filter)
{
	return QFileDialog::getOpenFileName(0,tr("选择文件"),QCoreApplication::applicationDirPath(),filter);//最后一个参数表示打开文件的格式
}

//文件信息：文件名
QString Utility::getFileNameWithExt(QString fileName)
{
	QFileInfo fileInfo(fileName);
	return fileInfo.fileName();
}

bool Utility::copyFile(QString srcFile, QString dstFile)
{
	if (isFileExist(dstFile))
	{
		if (ShowMessageBoxQuesion("该目录已经存在该文件，请问是否需要替换?") != 0)
		{
			return false;
		}
	}
	return QFile::copy(srcFile, dstFile);
}

void Utility::setStyle()
{        
	QString qss="";
	qss+="QLineEdit{border:1px solid gray;border-radius:7px;padding:3px;}";
	qss+="QListWidget{border:1px solid gray;}";
	qss+="QListWidget::item{padding:0px;margin:3px;}";
	qss+="QTreeWidget{border:1px solid gray;}";
	qss+="QTreeWidget::item{padding:0px;margin:3px;}";
	qss+="QTableWidget{border:1px solid gray;}";
	qss+="QTableView{border:1px solid gray;}";
	qss+="QScrollArea{border:1px solid gray;}";        
	qApp->setStyleSheet(qss);
}

//这个是居中函数，参数包含了屏幕的长宽，组件的长宽，怪不得程序中，每个窗口的打开都是居中的
void Utility::moveToCenter(QWidget *widget, int deskWidth, int deskHeight)
{
	int widgetX = widget->width();
	int widgetY = widget->height();
	QPoint movePoint(deskWidth/2 - widgetX / 2, deskHeight / 2 - widgetY / 2);
	widget->move(movePoint);
}

void Utility::setFixSize(QWidget *widget)
{
	widget->setFixedSize(widget->width(),widget->height());
}

void Utility::setWindowsMax(QWidget *widget)
{
	widget->setWindowState(Qt::WindowMaximized);
}
QMutex Utility::mutex;


//这个函数牛逼呀，有了很多事情呀，建立信号数据表插入采集信号的数据
void Utility::insertIntoDb(QString dataTime,QString areaNo, QString defenceType, QString hostNo, QString hostName, QString strData)
{
	QStringList tableName  = dataTime.split(" ");

	mutex.lock();//查询数据库操作都要上锁

	QSqlQuery query;
	query.exec(QString("show tables LIKE `%1_info`").arg(tableName[0]));//查询当前数据库中以_info开头的表。如果没有
	if (!query.next())                                                 //则建一个！
	{
		QSqlQuery createTableSQL;
		createTableSQL.exec(QString("CREATE TABLE `%1_info` (\
									`ID` int(11) NOT NULL AUTO_INCREMENT,\
									`Time` varchar(30) NOT NULL,\
									`AreaNo` varchar(10) NOT NULL,\
									`Type` varchar(10) NOT NULL,\
									`HostNo` varchar(10) NOT NULL,\
									`HostName` varchar(30) NOT NULL,\
									`Staff` varchar(30) NOT NULL,\
									`intTime` int(11) NOT NULL,\
									`Data` longtext NOT NULL,\
									PRIMARY KEY (`ID`),\
									UNIQUE KEY `idIndex` (`ID`) USING BTREE,\
									KEY `timeIndex` (`intTime`) USING BTREE \
									) ENGINE=InnoDB DEFAULT CHARSET=utf8;").arg(tableName[0]));
		createTableSQL.next();//这句话还是有作用的，并将查询定位在检索到的记录上


		/*createTableSQL.exec(QString("CREATE TABLE `%1_data` (\
									`ID` int(11) NOT NULL AUTO_INCREMENT,\
									`Data` longtext NOT NULL,\
									PRIMARY KEY (`ID`),\
									UNIQUE KEY `idIndex` (`ID`) USING BTREE\
									) ENGINE=InnoDB DEFAULT CHARSET=utf8;").arg(tableName[0]));*/
	}
	mutex.unlock();  

	QDateTime time;  
	time = QDateTime::fromString(dataTime, "yyyy-MM-dd hh:mm:ss");  
	int intTime = time.toTime_t();  //返回秒数

	mutex.lock();
	query.exec(QString("insert into `%1_info` (Time,AreaNo,Type,HostNo,HostName,Staff,intTime,Data) values('%2'").arg(tableName[0]).arg(tableName[1])
		+",'" 
		+ areaNo +"','" 
		+ defenceType + "','"
		+ hostNo + "','"
		+ hostName + "','"
		+ Config::currentUserName +"','"
		+ QString("%1").arg(intTime)+"','"
		+ strData+"')");
	query.next();
	//query.exec(QString("insert into `%1_data` (Data) values ('" + strData+ "')").arg(tableName[0]));
	mutex.unlock();
	
}

DefenceType Utility::getDefenceType(int &defencetype)
{
	switch (defencetype)
	{
	case 1:
		return DefenceType_1;
	case 2:
		return DefenceType_2;
	case 3:
		return DefenceType_3;
	case 4:
		return DefenceType_4;
	case 5:
		return DefenceType_5;
	}
}

DefenceStatus Utility::getDefenceStatus(QString &defenceStatus)
{
	return DefenceStatus_normal;
}