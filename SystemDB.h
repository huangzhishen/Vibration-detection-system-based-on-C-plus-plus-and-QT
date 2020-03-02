#ifndef SYSTEMDB_H
#define SYSTEMDB_H

#include <QString>
#include <QSqlDatabase>



class SystemDB
{
public:
	SystemDB(QString dataBase, QString hostName, QString dataBaseName, QString userName, QString userPassword, int port = 3306);
	~SystemDB();

	bool OpenDB();

private:
	QSqlDatabase db_;
};


#endif

