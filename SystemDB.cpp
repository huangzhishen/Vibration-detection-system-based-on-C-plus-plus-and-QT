#include "SystemDB.h"


SystemDB::SystemDB(QString dataBase, QString hostName, QString dataBaseName, QString userName, QString userPassword, int port)
{
	db_ = QSqlDatabase::addDatabase(dataBase);  	              //����
	db_.setHostName(hostName);  								  //���ݿ��ַ
	db_.setDatabaseName(dataBaseName);  						  //���ݿ���
	db_.setUserName(userName);  							      //�û���
	db_.setPassword(userPassword);  							  //����
	db_.setPort(port);  										  //�˿�
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
