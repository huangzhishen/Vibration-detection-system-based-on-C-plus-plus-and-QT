#include "CheckData.h"
#include "Utility.h"
#include "ShowData.h"
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>

//���������ť���鿴�ź�����
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
	ui_.txtWarnCount->setText(tr("          �ܹ����� %1 �θ��¼�").arg(mapWarnEvents_.size()));
	ui_.txtDangerCount->setText(tr("          �ܹ����� %1 �θ��¼�").arg(mapDangerEvents_.size()));
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

	//�Ż�  QDataTime ת��Ϊ uint���бȽ�, QStringת��ΪQDataTimeȻ����ת��Ϊuint
	QString sql(QString("select Data from `%1_info` where intTime='").arg(tableName[0]));
	QDateTime time;  
	time = QDateTime::fromString(currIndex, "yyyy-MM-dd hh:mm:ss"); 
	int intTime = time.toTime_t();
	sql += QString("%1").arg(intTime) + "' and AreaNo='";
	sql += mapWarnEvents_[currIndex] + "'";//�ɴ˿�֪��mapWarnEvents_���map�����������գ�ֵ�Ƿ�����

	Utility::mutex.lock();
	query.exec(sql);
	Utility::mutex.unlock();

	while (query.next())
	{
		float data2[10000];//��ʱ�������
		int size;
		QString strData2 = query.value(0).toString();//�ź�ֵ�ԡ������ָ���
		QStringList strList = strData2.split(",");
		size = strList.size();
		for (int i = 0 ; i < 10000; i++)
		{
			data2[i] = strList[i].toInt();//��ֵ
		}
		ShowData *w = new ShowData(data2,10000);//�����źŲ���ͼ
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


//���������źŵ��ź�
void CheckData::on_btnReset_clicked()
{
	ui_.warnTimeList->clear();
	ui_.dangerTimeList->clear();
	mapWarnEvents_.clear();
	mapDangerEvents_.clear();
	ui_.txtWarnCount->setText(tr("          �ܹ����� %1 �θ��¼�").arg(mapWarnEvents_.size()));
	ui_.txtDangerCount->setText(tr("          �ܹ����� %1 �θ��¼�").arg(mapDangerEvents_.size()));
	emit resetSignal();
}