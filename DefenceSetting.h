#ifndef DEFENCESETTING_H
#define DEFENCESETTING_H
#include <QtGui/QDialog>
#include "ui_defenceSetting.h"
#include "Defencebutton.h"
#include "Config.h"


class DefenceSetting :public QDialog
{
	Q_OBJECT
public:
	DefenceSetting(QDialog *parent = 0);
	~DefenceSetting();

protected:
	bool eventFilter(QObject *obj, QEvent *event);
private slots:
	void on_btnAdd_clicked();
	void on_btnDelete_clicked();
	void on_btnImportMap_clicked();
	void on_btnModifty_clicked();

	void on_txtDefenceImage_currentIndexChanged(const QString &arg1);
	void on_treeDefence_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
	Ui::DefenceSetting ui_;
	bool isAddOrUpdateDefence_; 
	DefenceButton *btnTemp_;
	QString tempDefenceID_;
	QString tempHostID_;
	QStringList	hostID_;
	QStringList hostName_;
	QStringList defenceIDHostID_;


	void initForm();
	void getImageFile();
	void getSoundFile();
	void readHostInfo();
	void readDefenceInfo();
	void initDefenceArea();
	void setDefenceButtonVisible();

	bool checkDefenceInfo(QString hostID, QString hostName, QString defenceImage, QString posX, QString posY);
	bool isExistDefence(QString defenceID, QString hostID);

};


#endif
