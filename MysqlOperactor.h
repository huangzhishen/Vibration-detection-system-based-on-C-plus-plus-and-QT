#ifndef MYSQLOPERACTOR_H
#define MYSQLOPERACTOR_H

#include <QtSql>
#include <QTableView>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QObject>


class MysqlOperactor:public QObject
{
	Q_OBJECT
public:
	explicit MysqlOperactor(QObject *parent = 0);
	~MysqlOperactor();
	
	void controlTableView(QTableView *tableView, QLabel *label, QPushButton *btnFirst, QPushButton *btnPrev, QPushButton *btnNext, QPushButton *btnLast);
	void bindDataToCbox(QString columnName, QString tableName, QComboBox *cbox);
	bool bindDataToTableView(QString tableName, QString orderColumn, QString orderType, int rows, QString columnNames[], int columnWidth[]);
	void updateTableView(QString sql);
	void searchData(QString eventTime,QString eventArea,QString eventType, QString eventHost,QString eventStaff);
	
private slots:
	void firstClick();
	void prevClick();
	void nextClick();
	void lastClick();
};


#endif

