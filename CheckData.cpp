#include "CheckData.h"
#include "Utility.h"
#include "ShowData.h"
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>

//点击防区按钮，查看信号数据
CheckData::CheckData(QMap<QString,QString> mapWarnEvents,QMap<QString,QString> mapDangerEvents, const char* title, QDialog *parent)
	:mapWarnEvents_(mapWarnEvents),mapDangerEvents_(mapDangerEvents),QDialog(parent)
{
	ui_.setupUi(this);
	foreach(QString currtime, mapWarnEvents_.keys())
	{
		ui_.warnTimeList->addItem(currtime);
	}
	foreach(QString currtime, mapDangerEvents_.keys())
	{
		ui_.dangerTimeList->addItem(currtime);
	}
	ui_.txtWarnCount->setText(tr("          总共发生 %1 次该事件").arg(mapWarnEvents_.size()));
	ui_.txtDangerCount->setText(tr("          总共发生 %1 次该事件").arg(mapDangerEvents_.size()));
	this->setWindowTitle(tr(title));
}
CheckData::~CheckData(void)
{

}



void CheckData::on_btnWarnQuery_clicked()
{
	QString currIndex = ui_.warnTimeList->currentText();
//	qDebug() << currIndex << " " <<  mapWarnEvents_[currIndex];
	QSqlQuery query;
	QStringList tableName  = currIndex.split(" ");

	//优化  QDataTime 转换为 uint进行比较, QString转换为QDataTime然后在转换为uint
	QString sql(QString("select Data from `%1_info` where intTime='").arg(tableName[0]));
	QDateTime time;  
	time = QDateTime::fromString(currIndex, "yyyy-MM-dd hh:mm:ss"); 
	int intTime = time.toTime_t();
	sql += QString("%1").arg(intTime) + "' and AreaNo='";
	sql += mapWarnEvents_[currIndex] + "'";//由此可知，mapWarnEvents_这个map索引是年月日，值是防区号

	Utility::mutex.lock();
	query.exec(sql);
	Utility::mutex.unlock();

	while (query.next())
	{
		float data2[10000];//临时数组变量
		int size;
		QString strData2 = query.value(0).toString();//信号值以“，”分隔。
		QStringList strList = strData2.split(",");
		size = strList.size();
		for (int i = 0 ; i < 10000; i++)
		{
			data2[i] = strList[i].toInt();//赋值
		}
		ShowData *w = new ShowData(data2,10000);//画出信号波形图
		w->setAttribute(Qt::WA_DeleteOnClose);
		w->show();
	}
}

void CheckData::on_btnDangerQuery_clicked()
{
	QString currIndex = ui_.dangerTimeList->currentText();
	//qDebug() << currIndex << " " <<  mapDangerEvents_[currIndex];
	QStringList tableName  = currIndex.split(" ");
	QSqlQuery query;
	QString sql(QString("select Data from `%1_info` where intTime='").arg(tableName[0]));
	QDateTime time;  
	time = QDateTime::fromString(currIndex, "yyyy-MM-dd hh:mm:ss"); 
	int intTime = time.toTime_t();
	sql += QString("%1").arg(intTime) + "' and AreaNo='";
	sql += mapDangerEvents_[currIndex] + "'";

	Utility::mutex.lock();
	query.exec(sql);
	Utility::mutex.unlock();

	while (query.next())
	{
		float data2[10000];
		int size;
		QString strData2 = query.value(0).toString();
		QStringList strList = strData2.split(",");
		size = strList.size();
		for (int i = 0 ; i < 10000; i++)
		{
			data2[i] = strList[i].toInt();
		}
		ShowData *w = new ShowData(data2,10000);
		w->setAttribute(Qt::WA_DeleteOnClose);
		w->show();
	}
}


//发送重置信号的信号
void CheckData::on_btnReset_clicked()
{
	ui_.warnTimeList->clear();
	ui_.dangerTimeList->clear();
	mapWarnEvents_.clear();
	mapDangerEvents_.clear();
	ui_.txtWarnCount->setText(tr("          总共发生 %1 次该事件").arg(mapWarnEvents_.size()));
	ui_.txtDangerCount->setText(tr("          总共发生 %1 次该事件").arg(mapDangerEvents_.size()));
	emit resetSignal();
}