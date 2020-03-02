#include "MysqlOperactor.h"
#include <QHeaderView>//这个头文件关于视图的头行头列的操作
#include <QAbstractItemView>//这个头文件关于模型视图：SqlQueryModel *QueryModel;//模型
#include "Utility.h"

QTableView *TableView;//视图

QLabel *Label;
QPushButton *BtnFirst;//第一页
QPushButton *BtnPrev;
QPushButton *BtnNext;
QPushButton *BtnLast;

QSqlQueryModel *QueryModel;//模型

int StartIndex;
int CurrRow;
int RowsCount;
int CurrPage;
int PageCount;

QString TmpSqlQuery;
QString TmpSqlLimit;

MysqlOperactor::MysqlOperactor(QObject *parent)
	:QObject(parent)
{
	StartIndex = 0;
	CurrRow = 0;
	RowsCount = 0;
	CurrPage = 1;
	PageCount = 0;
	TmpSqlQuery = "";
	TmpSqlLimit = "";
}

MysqlOperactor::~MysqlOperactor()
{
}

//设置模型，放入view，设置第一页、下一页、上一页、最后一页、按钮的connect连接。
void MysqlOperactor::controlTableView(QTableView *tableView, QLabel *label, QPushButton *btnFirst, QPushButton *btnPrev, QPushButton *btnNext, QPushButton *btnLast)
{
	TableView = tableView;
	Label = label;
	BtnFirst = btnFirst;
	BtnPrev = btnPrev;
	BtnNext = btnNext;
	BtnLast = btnLast;

	QueryModel = new QSqlQueryModel(TableView);
	connect(BtnFirst,SIGNAL(clicked()), this, SLOT(firstClick()));
	connect(BtnPrev, SIGNAL(clicked()), this, SLOT(prevClick()));
	connect(BtnNext, SIGNAL(clicked()), this, SLOT(nextClick()));
	connect(BtnLast, SIGNAL(clicked()), this, SLOT(lastClick()));

}

//从数据库根据columnName提取数据到下拉框中，这里还不知道是什么数据？
void MysqlOperactor::bindDataToCbox(QString columnName, QString tableName, QComboBox *cbox)
{
	QSqlQuery query;//新建一个数据库操作
	Utility::mutex.lock();
	query.exec("select "+ columnName + " from `" + tableName + "` order by " + columnName + " asc ");
	Utility::mutex.unlock();
	while (query.next())
	{
		cbox->addItem(query.value(0).toString());
	}
}

//模型更改了，更新视图，原来这个师兄自己写的更新视图函数，同时更新标签显示有多少条数据，当前页、总页数！
void MysqlOperactor::updateTableView(QString sql)
{
	Utility::mutex.lock();
	QueryModel->setQuery(sql);
	TableView->setModel(QueryModel);
	Utility::mutex.unlock();
	Label->setText(tr("共有 %1 条数据   共 %2 页   当前第 %3 页").arg(RowsCount).arg(PageCount).arg(CurrPage));
}

//这个函数有点诡异，一通过简单计算完成对页数，总数据数的确定，二完成了对前row行数据的显示，三是设置一些视图的颜色等等。
bool MysqlOperactor::bindDataToTableView(QString tableName, QString orderColumn, QString orderType, int rows, QString columnNames[], int columnWidth[])
{
	StartIndex = 0;//用处不大
	CurrPage = 1;//有点作用
	CurrRow = rows;//在槽函数中有用到。
	
	TmpSqlQuery = "select id from `" + tableName;
	TmpSqlQuery = TmpSqlQuery + "` ORDER BY id desc limit 1";//limit 1指的是取排序后的第一个

	QSqlQuery query;

	//query.prepare();
	Utility::mutex.lock();
	bool ret = query.exec(TmpSqlQuery);
	Utility::mutex.unlock();
	query.next();//指向当前行的数据
	RowsCount = query.value(0).toInt();//更新标签

	if (RowsCount % rows == 0)//更新标签
	{
		PageCount = RowsCount / rows;
	}
	else{
		PageCount = RowsCount / rows + 1;
	}

	//优化   select * from datainfo where id > ? ORDER BY id limit 15 (利用索引);
	//全部导入
	TmpSqlQuery = "select ID,Time,AreaNo,Type,HostNo,HostName,Staff from `" + tableName + "` order by " + orderColumn + " " + orderType;
	TmpSqlLimit = QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(rows);
	updateTableView(TmpSqlLimit);//更新视图


	for (int i=0;i < TableView->model()->columnCount();i++){
		QueryModel->setHeaderData(i,Qt::Horizontal,columnNames[i]);//设置列标题
		TableView->setColumnWidth(i,columnWidth[i]);//设置列宽
	}

	TableView->horizontalHeader()->setHighlightSections(false);//点击表头时不对表头光亮
	TableView->setSelectionMode(QAbstractItemView::SingleSelection);//选中模式为单行选中
	TableView->setSelectionBehavior(QAbstractItemView::SelectRows);//选中整行

	//这里是设置视图的背景颜色等。
	TableView->setStyleSheet( "QTableView::item:hover{background-color:rgb(0,255,0,50)}"
		"QTableView::item:selected{background-color:#008B00}");
	return ret;
}

//以下四个函数是对四个按钮的槽函数
void MysqlOperactor::firstClick()
{
	if (CurrPage > 1){
		StartIndex=0;
		CurrPage = 1;
		TmpSqlLimit=QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(CurrRow);//组织分页SQL语句
		updateTableView(TmpSqlLimit);
	}
}
void MysqlOperactor::prevClick()
{
	if (CurrPage > 1){
		StartIndex -= CurrRow;
		CurrPage--;
		TmpSqlLimit=QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(CurrRow);//组织分页SQL语句
		updateTableView(TmpSqlLimit);
	}
}
void MysqlOperactor::nextClick()
{
	if (CurrPage < PageCount){
		StartIndex += CurrRow;
		CurrPage++;
		TmpSqlLimit=QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(CurrRow);//组织分页SQL语句
		updateTableView(TmpSqlLimit);
	}
}
void MysqlOperactor::lastClick()
{
	if (CurrPage < PageCount){
		StartIndex = (PageCount-1)*CurrRow;
		CurrPage = PageCount;
	
		TmpSqlLimit=QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(CurrRow);//组织分页SQL语句
		updateTableView(TmpSqlLimit);
	}
}


//这个函数就是检索在eventtime，eventarea，eventtypr,host,staff下的数据。
void MysqlOperactor::searchData(QString eventTime,QString eventArea,QString eventType, QString eventHost,QString eventStaff)
{
	StartIndex = 0;
	CurrPage = 1;
	int rows = 15;

	CurrRow = rows;

	QString tableName;

	if (eventTime.isEmpty())
	{
		tableName = QDate::currentDate().toString("yyyy-MM-dd");
	}
	else
	{
		tableName = eventTime;
	}
	tableName += "_info";

	TmpSqlQuery = "select count(*) from `" + tableName + "`";  

	if (!eventArea.isEmpty())
	{
		TmpSqlQuery.append(QString(" where AreaNo=%1").arg(eventArea));  
	}
	if (!eventType.isEmpty())
	{
		TmpSqlQuery.append(QString(" where Type=%1").arg(eventType));
	}

	QSqlQuery query;   
	Utility::mutex.lock();   
	bool ret = query.exec(TmpSqlQuery);   //第一个查询主要是得到该条件下的数据总数
	Utility::mutex.unlock();   
	query.next();   
	RowsCount = query.value(0).toInt();   

	if (RowsCount % rows == 0)
	{
		PageCount = RowsCount / rows;
	}
	else{
		PageCount = RowsCount / rows + 1;
	}

	//第二个查询就是得到该条件下的全部数据
	TmpSqlQuery = "select ID,Time,AreaNo,Type,HostNo,hostName,Staff from `" + tableName + "`";
	if (!eventArea.isEmpty())
	{
		TmpSqlQuery.append(QString(" where AreaNo=%1").arg(eventArea));
	}
	if (!eventType.isEmpty())
	{
		TmpSqlQuery.append(QString(" where Type=%1").arg(eventType));
	}

	TmpSqlLimit = QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(rows);//从第1条开始，取15条
	updateTableView(TmpSqlLimit);

}