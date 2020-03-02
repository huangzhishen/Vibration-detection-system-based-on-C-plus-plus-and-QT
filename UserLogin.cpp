#include <QSqlQuery>
#include <QEvent>
#include <QKeyEvent>
#include "UserLogin.h"
#include "Utility.h"
#include "Config.h"
#include "SecuritySystem.h"

UserLogin::UserLogin(QDialog *parent)
	:QDialog(parent)
{
	ui_.setupUi(this);
	Utility::moveToCenter(this,Config::DeskWidth,Config::DeskHeigth);
	Utility::setFixSize(this);
	initForm();
}

UserLogin::~UserLogin()
{
}

void UserLogin::initForm()
{
	//�����û���Ϣ�����ڴ�,���û�ѡ���û�������������ʱ,���ڴ��бȽ�����,�ٶȼ���.
	QSqlQuery query;
    query.exec("select * from userInfo");

    while(query.next()){
        ui_.txtUserName->addItem(query.value(0).toString());
        //����Ӧ�û�����������ʹ�����������,���û���¼ʱ�Ա�,�ȴ����ݿ��в�ѯ�ٶ�Ҫ��.
		userPwd_ << query.value(1).toString();
        userType_ << query.value(2).toString();
    }

    //��Ӧ��ʾ�������͹�������Ϣ.
    ui_.labTitle->setText("Security System");
    ui_.labCompany->setText(QString("Technology support :%1").arg("scut"));
    ui_.txtUserName->setCurrentIndex(ui_.txtUserName->findText("admin"));    

    //��������̴���
    ui_.gboxSoftKey->setGeometry(30,76,261,61);
    ui_.gboxSoftKey->setVisible(false);

	//�Ż� �ڴ�й©
    QList<QPushButton *> btn=ui_.gboxSoftKey->findChildren<QPushButton *>();//���д���ѧ��������ѽ����������µ�children��ť���һ��List
    foreach (QPushButton *b, btn)
	{
        connect(b,SIGNAL(clicked()),this,SLOT(buttonClick()));
    }
}

//������������������û����Լ��̰�����½���õ����¼�
bool UserLogin::eventFilter(QObject *obj, QEvent *event)
{
	//�û����»س���,������¼�ź�.
	if (event->type()==QEvent::KeyPress)
	{
		QKeyEvent *keyEvent=static_cast<QKeyEvent *>(event);
		if (keyEvent->key()==Qt::Key_Return || keyEvent->key()==Qt::Key_Enter) 
		{
			this->on_btnLogin_clicked();
			return true;
		}
	}
	return QObject::eventFilter(obj,event);
}

//��½��ť�Ĳۺ�����������newһ��SecritySystem�Ĵ���
void UserLogin::on_btnLogin_clicked()
{
	QString UserPwd = ui_.txtUserPwd->text();
	if (UserPwd=="")
	{
		Utility::ShowMessageBoxError("���벻��Ϊ�գ�����������");
		ui_.txtUserPwd->setFocus();//��������ڼ����������봦
		return;
	}

	int index=ui_.txtUserName->currentIndex();
	if (UserPwd == userPwd_[index])
	{
		Config::lastLoginUser = ui_.txtUserName->currentText();
		Config::currentUserName = Config::lastLoginUser;
		Config::currentUserPassword = UserPwd;
		Config::currentUserType = userType_[index];
		//�Ż� �ڴ�й©
		SecuritySystem *w = new SecuritySystem();
		w->show();
		this->hide();
	}else{
		Utility::ShowMessageBoxError("�����������������");
		ui_.txtUserPwd->setFocus();
	}
}

void UserLogin::on_btnClose_clicked()
{
	qApp->quit();
}

void UserLogin::buttonClick()
{
	QPushButton *btn=(QPushButton *)sender();//sender()�ҵ��ĸ����󴥷��òۺ���
	QString objectName=btn->objectName();
	QString text=ui_.txtUserPwd->text();

	if (objectName=="btnDelete"){
		if (text.length()>0){
			ui_.txtUserPwd->setText(text.mid(0,text.length()-1));//Mid()����Ҳ�Ǻ�ǿ��ģ�ȡ0��length-1���ַ���
		}
	}else if (objectName=="btnQ"){
		ui_.txtUserPwd->setText(text+"Q");
	}else if (objectName=="btnW"){
		ui_.txtUserPwd->setText(text+"W");
	}else if (objectName=="btnE"){
		ui_.txtUserPwd->setText(text+"E");
	}else if (objectName=="btnR"){
		ui_.txtUserPwd->setText(text+"R");
	}else if (objectName=="btnT"){
		ui_.txtUserPwd->setText(text+"T");
	}else if (objectName=="btnY"){
		ui_.txtUserPwd->setText(text+"Y");
	}else if (objectName=="btnU"){
		ui_.txtUserPwd->setText(text+"U");
	}else if (objectName=="btnI"){
		ui_.txtUserPwd->setText(text+"I");
	}else if (objectName=="btnO"){
		ui_.txtUserPwd->setText(text+"O");
	}else if (objectName=="btnP"){
		ui_.txtUserPwd->setText(text+"P");
	}else if (objectName=="btnA"){
		ui_.txtUserPwd->setText(text+"A");
	}else if (objectName=="btnS"){
		ui_.txtUserPwd->setText(text+"S");
	}else if (objectName=="btnD"){
		ui_.txtUserPwd->setText(text+"D");
	}else if (objectName=="btnF"){
		ui_.txtUserPwd->setText(text+"F");
	}else if (objectName=="btnG"){
		ui_.txtUserPwd->setText(text+"G");
	}else if (objectName=="btnH"){
		ui_.txtUserPwd->setText(text+"H");
	}else if (objectName=="btnJ"){
		ui_.txtUserPwd->setText(text+"J");
	}else if (objectName=="btnK"){
		ui_.txtUserPwd->setText(text+"K");
	}else if (objectName=="btnL"){
		ui_.txtUserPwd->setText(text+"L");
	}else if (objectName=="btnZ"){
		ui_.txtUserPwd->setText(text+"Z");
	}else if (objectName=="btnX"){
		ui_.txtUserPwd->setText(text+"X");
	}else if (objectName=="btnC"){
		ui_.txtUserPwd->setText(text+"C");
	}else if (objectName=="btnV"){
		ui_.txtUserPwd->setText(text+"V");
	}else if (objectName=="btnB"){
		ui_.txtUserPwd->setText(text+"B");
	}else if (objectName=="btnN"){
		ui_.txtUserPwd->setText(text+"N");
	}else if (objectName=="btnM"){
		ui_.txtUserPwd->setText(text+"M");
	}else if (objectName=="btn0"){
		ui_.txtUserPwd->setText(text+"0");
	}else if (objectName=="btn1"){
		ui_.txtUserPwd->setText(text+"1");
	}else if (objectName=="btn2"){
		ui_.txtUserPwd->setText(text+"2");
	}else if (objectName=="btn3"){
		ui_.txtUserPwd->setText(text+"3");
	}else if (objectName=="btn4"){
		ui_.txtUserPwd->setText(text+"4");
	}else if (objectName=="btn5"){
		ui_.txtUserPwd->setText(text+"5");
	}else if (objectName=="btn6"){
		ui_.txtUserPwd->setText(text+"6");
	}else if (objectName=="btn7"){
		ui_.txtUserPwd->setText(text+"7");
	}else if (objectName=="btn8"){
		ui_.txtUserPwd->setText(text+"8");
	}else if (objectName=="btn9"){
		ui_.txtUserPwd->setText(text+"9");
	}
}

//�����ť����С���̣������򵯳������
void UserLogin::on_btnKey_clicked()
{
	ui_.gboxSoftKey->setVisible(!ui_.gboxSoftKey->isVisible());
}