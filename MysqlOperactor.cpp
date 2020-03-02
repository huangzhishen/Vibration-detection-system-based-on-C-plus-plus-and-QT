#include "MysqlOperactor.h"
#include <QHeaderView>//���ͷ�ļ�������ͼ��ͷ��ͷ�еĲ���
#include <QAbstractItemView>//���ͷ�ļ�����ģ����ͼ��SqlQueryModel *QueryModel;//ģ��
#include "Utility.h"

QTableView *TableView;//��ͼ

QLabel *Label;
QPushButton *BtnFirst;//��һҳ
QPushButton *BtnPrev;
QPushButton *BtnNext;
QPushButton *BtnLast;

QSqlQueryModel *QueryModel;//ģ��

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

//����ģ�ͣ�����view�����õ�һҳ����һҳ����һҳ�����һҳ����ť��connect���ӡ�
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

//�����ݿ����columnName��ȡ���ݵ��������У����ﻹ��֪����ʲô���ݣ�
void MysqlOperactor::bindDataToCbox(QString columnName, QString tableName, QComboBox *cbox)
{
	QSqlQuery query;//�½�һ�����ݿ����
	Utility::mutex.lock();
	query.exec("select "+ columnName + " from `" + tableName + "` order by " + columnName + " asc ");
	Utility::mutex.unlock();
	while (query.next())
	{
		cbox->addItem(query.value(0).toString());
	}
}

//ģ�͸����ˣ�������ͼ��ԭ�����ʦ���Լ�д�ĸ�����ͼ������ͬʱ���±�ǩ��ʾ�ж��������ݣ���ǰҳ����ҳ����
void MysqlOperactor::updateTableView(QString sql)
{
	Utility::mutex.lock();
	QueryModel->setQuery(sql);
	TableView->setModel(QueryModel);
	Utility::mutex.unlock();
	Label->setText(tr("���� %1 ������   �� %2 ҳ   ��ǰ�� %3 ҳ").arg(RowsCount).arg(PageCount).arg(CurrPage));
}

//��������е���죬һͨ���򵥼�����ɶ�ҳ��������������ȷ����������˶�ǰrow�����ݵ���ʾ����������һЩ��ͼ����ɫ�ȵȡ�
bool MysqlOperactor::bindDataToTableView(QString tableName, QString orderColumn, QString orderType, int rows, QString columnNames[], int columnWidth[])
{
	StartIndex = 0;//�ô�����
	CurrPage = 1;//�е�����
	CurrRow = rows;//�ڲۺ��������õ���
	
	TmpSqlQuery = "select id from `" + tableName;
	TmpSqlQuery = TmpSqlQuery + "` ORDER BY id desc limit 1";//limit 1ָ����ȡ�����ĵ�һ��

	QSqlQuery query;

	//query.prepare();
	Utility::mutex.lock();
	bool ret = query.exec(TmpSqlQuery);
	Utility::mutex.unlock();
	query.next();//ָ��ǰ�е�����
	RowsCount = query.value(0).toInt();//���±�ǩ

	if (RowsCount % rows == 0)//���±�ǩ
	{
		PageCount = RowsCount / rows;
	}
	else{
		PageCount = RowsCount / rows + 1;
	}

	//�Ż�   select * from datainfo where id > ? ORDER BY id limit 15 (��������);
	//ȫ������
	TmpSqlQuery = "select ID,Time,AreaNo,Type,HostNo,HostName,Staff from `" + tableName + "` order by " + orderColumn + " " + orderType;
	TmpSqlLimit = QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(rows);
	updateTableView(TmpSqlLimit);//������ͼ


	for (int i=0;i < TableView->model()->columnCount();i++){
		QueryModel->setHeaderData(i,Qt::Horizontal,columnNames[i]);//�����б���
		TableView->setColumnWidth(i,columnWidth[i]);//�����п�
	}

	TableView->horizontalHeader()->setHighlightSections(false);//�����ͷʱ���Ա�ͷ����
	TableView->setSelectionMode(QAbstractItemView::SingleSelection);//ѡ��ģʽΪ����ѡ��
	TableView->setSelectionBehavior(QAbstractItemView::SelectRows);//ѡ������

	//������������ͼ�ı�����ɫ�ȡ�
	TableView->setStyleSheet( "QTableView::item:hover{background-color:rgb(0,255,0,50)}"
		"QTableView::item:selected{background-color:#008B00}");
	return ret;
}

//�����ĸ������Ƕ��ĸ���ť�Ĳۺ���
void MysqlOperactor::firstClick()
{
	if (CurrPage > 1){
		StartIndex=0;
		CurrPage = 1;
		TmpSqlLimit=QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(CurrRow);//��֯��ҳSQL���
		updateTableView(TmpSqlLimit);
	}
}
void MysqlOperactor::prevClick()
{
	if (CurrPage > 1){
		StartIndex -= CurrRow;
		CurrPage--;
		TmpSqlLimit=QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(CurrRow);//��֯��ҳSQL���
		updateTableView(TmpSqlLimit);
	}
}
void MysqlOperactor::nextClick()
{
	if (CurrPage < PageCount){
		StartIndex += CurrRow;
		CurrPage++;
		TmpSqlLimit=QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(CurrRow);//��֯��ҳSQL���
		updateTableView(TmpSqlLimit);
	}
}
void MysqlOperactor::lastClick()
{
	if (CurrPage < PageCount){
		StartIndex = (PageCount-1)*CurrRow;
		CurrPage = PageCount;
	
		TmpSqlLimit=QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(CurrRow);//��֯��ҳSQL���
		updateTableView(TmpSqlLimit);
	}
}


//����������Ǽ�����eventtime��eventarea��eventtypr,host,staff�µ����ݡ�
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
	bool ret = query.exec(TmpSqlQuery);   //��һ����ѯ��Ҫ�ǵõ��������µ���������
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

	//�ڶ�����ѯ���ǵõ��������µ�ȫ������
	TmpSqlQuery = "select ID,Time,AreaNo,Type,HostNo,hostName,Staff from `" + tableName + "`";
	if (!eventArea.isEmpty())
	{
		TmpSqlQuery.append(QString(" where AreaNo=%1").arg(eventArea));
	}
	if (!eventType.isEmpty())
	{
		TmpSqlQuery.append(QString(" where Type=%1").arg(eventType));
	}

	TmpSqlLimit = QString("%1 limit %2,%3;").arg(TmpSqlQuery).arg(StartIndex).arg(rows);//�ӵ�1����ʼ��ȡ15��
	updateTableView(TmpSqlLimit);

}