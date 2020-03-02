#ifndef CHECKDATA_H
#define CHECKDATA_H
#include <QDialog>
#include <QMap>
#include <QString>
#include "ui_checkData.h"

class CheckData:public QDialog
{
	Q_OBJECT
public:
	CheckData(QMap<QString,QString> mapWarnEvents,QMap<QString,QString> mapDangerEvents, const char* title ,QDialog *parent = 0);
	~CheckData(void);

private slots:
	void on_btnWarnQuery_clicked();
	void on_btnDangerQuery_clicked();
	void on_btnReset_clicked();

signals:
	void resetSignal();

private:
	Ui::checkData ui_;
	QMap<QString,QString> mapWarnEvents_;
	QMap<QString,QString> mapDangerEvents_;
};


#endif
