#include "SystemDB.h"


SystemDB::SystemDB(QString dataBase, QString hostName, QString dataBaseName, QString userName, QString userPassword, int port)
{
	db_ = QSqlDatabase::addDatabase(dataBase);  	              //驱动
	db_.setHostName(hostName);  								  //数据库地址
	db_.setDatabaseName(dataBaseName);  						  //数据库名
	db_.setUserName(userName);  							      //用户名
	db_.setPassword(userPassword);  							  //密码
	db_.setPort(port);  										  //端口
}

bool SystemDB::OpenDB()
{
	if (db_.isOpen())
	{
		return true;
	}
	return db_.open();
}

SystemDB::~SystemDB()
{
	if(db_.isOpen()){
		db_.close();
	}
}
