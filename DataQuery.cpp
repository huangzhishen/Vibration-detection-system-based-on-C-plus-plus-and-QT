#include "DataQuery.h"
#include "Utility.h"
#include "Config.h"
#include "ShowData.h"
#include <QDebug>
#include <memory>
#include <QDateTime>
DataQuery::DataQuery(QDialog *parent)
	:QDialog(parent)
{
	ui_.setupUi(this);
	Utility::moveToCenter(this,Config::DeskWidth,Config::DeskHeigth);
	Utility::setFixSize(this);
	initForm();
}


DataQuery::~DataQuery(void)
{
}

void DataQuery::initForm()
{
	columnNames_[0]="ID";
	columnNames_[1]="Time";
    columnNames_[2]="AreaNo";
    columnNames_[3]="Type";
    columnNames_[4]="HostNo";
    columnNames_[5]="HostName";
	columnNames_[6]="Staff";

    columnWidths_[0]=100;
    columnWidths_[1]=150;
    columnWidths_[2]=80;
    columnWidths_[3]=80;
    columnWidths_[4]=80;
    columnWidths_[5]=150;
	columnWidths_[6]=100;

	ui_.ckTriggerTime->setChecked(false);
	ui_.ckTriggerHost->setChecked(false);
	ui_.ckTriggerType->setChecked(false);
	ui_.ckDefenceName->setChecked(false);
	ui_.ckUserName->setChecked(false);


	ui_.dateStart->setEnabled(false);
	ui_.dateEnd->setEnabled(false);
	ui_.cboxTriggerHost->setEnabled(false);
	ui_.cboxTriggerType->setEnabled(false);
	ui_.cboxDefenceName->setEnabled(false);
	ui_.cboxUserName->setEnabled(false);

	ui_.dateStart->setDate(QDate::currentDate());
	ui_.dateEnd->setDate(QDate::currentDate());

	ui_.dateStart->setDisplayFormat(QString("yyyy.MM.dd"));
	ui_.dateEnd->setDisplayFormat(QString("yyyy.MM.dd"));
	
	for (int i = 1; i <= 3 ; i++ )//类型勾选下拉框中循环赋值
	{
		ui_.cboxTriggerType->addItem(QString("%1").arg(i));
	}

	//ui_.tableEvent->horizontalHeader()->setStretchLastSection(true);


	mysql = new MysqlOperactor(this);//新建一个M类
	mysql->bindDataToCbox("hostID"	,"hostInfo",ui_.cboxTriggerHost);//导入数据
	mysql->bindDataToCbox("defenceID","defenceInfo",ui_.cboxDefenceName);//导入数据
	mysql->bindDataToCbox("userName","userInfo",ui_.cboxUserName);//导入数据
	mysql->controlTableView(ui_.tableEvent, ui_.labInfo, ui_.btnFirst, ui_.btnPre, ui_.btnNext, ui_.btnLast);
	//QDate curDate = QDate::currentDate().DateFor;
	QString tableName = QDate::currentDate().toString("yyyy-MM-dd");
	tableName += "_info";

	bool ret = mysql->bindDataToTableView(tableName,"id","desc",15,columnNames_,columnWidths_);
	if (ret == false)
	{
		return ;
	}

	ui_.tableEvent->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);//插入成功
	ui_.tableEvent->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setResizeMode(2,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setResizeMode(3,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setResizeMode(4,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setResizeMode(5,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setResizeMode(6,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

	
}

//警情查询上的五个勾选项的框
void DataQuery::on_ckTriggerTime_toggled(bool checked)
{
	ui_.dateEnd->setEnabled(checked);
	ui_.dateStart->setEnabled(checked);
}

void DataQuery::on_ckTriggerHost_toggled(bool checked)
{
	ui_.cboxTriggerHost->setEnabled(checked);
}

void DataQuery::on_ckTriggerType_toggled(bool checked)
{
	ui_.cboxTriggerType->setEnabled(checked);
}

void DataQuery::on_ckDefenceName_toggled(bool checked)
{
	ui_.cboxDefenceName->setEnabled(checked);
}

void DataQuery::on_ckUserName_toggled(bool checked)
{
	ui_.cboxUserName->setEnabled(checked);
}

//警情查询那里的四个按钮的槽函数
void DataQuery::on_btnSelect_clicked()
{
	QString searchTime;
	QString searchArea;
	QString searchType;
	QString searchHost;
	QString searchStaff;

	if (ui_.ckTriggerTime->isChecked())
	{
		QString tmp = ui_.dateStart->text();
		QStringList tmpList = tmp.split(".");//把时间的.去掉
		searchTime.append(QString("%1").arg(tmpList[0]));
		for (int i = 1 ; i < tmpList.size(); i++)
		{
			searchTime.append(QString("-%1").arg(tmpList[i]));//给searchTime赋值
		}
	}
	if (ui_.ckDefenceName->isChecked())
	{
		searchArea = ui_.cboxDefenceName->currentText();
	}
	if (ui_.ckTriggerType->isChecked())
	{
		searchType = ui_.cboxTriggerType->currentText();
	}

	mysql->searchData(searchTime,searchArea,searchType,searchHost,searchStaff);
	ui_.tableEvent->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);//插入失败
	ui_.tableEvent->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setResizeMode(2,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setResizeMode(3,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setResizeMode(4,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setResizeMode(5,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setResizeMode(6,QHeaderView::Stretch);
	ui_.tableEvent->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
}

void DataQuery::on_btnDelete_clicked()
{

}

void DataQuery::on_btnData_clicked()
{
	int row= ui_.tableEvent->currentIndex().row();
	QAbstractItemModel *model = ui_.tableEvent->model ();
	QModelIndex index = model->index(row,0);
	QVariant data = model->data(index);
	
	QSqlQuery query;
	QString tableName = QDate::currentDate().toString("yyyy-MM-dd");
	tableName += "_info";
	Utility::mutex.lock();
	query.exec(QString("select data from `%1` where id='").arg(tableName) + data.toString() + "'");
	Utility::mutex.unlock();

	float data2[10000];
	int size;
	while (query.next())
	{	
		QString strData2 = query.value(0).toString();
		QStringList strList = strData2.split(",");
		size = strList.size();
		for (int i = 0 ; i < 10000; i++)
		{
			data2[i] = strList[i].toInt();
		}
	}
	ShowData *w = new ShowData(data2,10000);
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
}

void DataQuery::on_btnExcel_clicked()
{

}

