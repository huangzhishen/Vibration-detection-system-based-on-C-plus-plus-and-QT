#ifndef DATAQUERY_H
#define DATAQUERY_H
#include <QDialog>
#include "ui_dataQuery.h"
#include "MysqlOperactor.h"

class DataQuery:public QDialog
{
	Q_OBJECT
public:
	DataQuery(QDialog *parent = 0);
	~DataQuery(void);

private slots:	
	void on_ckTriggerTime_toggled(bool checked);    //ʱ���л�
	void on_ckTriggerHost_toggled(bool checked); //�����л�
	void on_ckTriggerType_toggled(bool checked);    //�����л�
	void on_ckDefenceName_toggled(bool checked);    //�������л�
	void on_ckUserName_toggled(bool checked);       //ֵ��Ա�л�

	void on_btnSelect_clicked();
	void on_btnDelete_clicked();
	void on_btnData_clicked();
	void on_btnExcel_clicked();

	

private:
	Ui::dataQuery ui_;
	QString columnNames_[7];
	int columnWidths_[7];
	MysqlOperactor *mysql;
	void initForm();

};


#endif
