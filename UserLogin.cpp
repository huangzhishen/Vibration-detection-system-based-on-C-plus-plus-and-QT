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
	//加载用户信息链表到内存,当用户选择用户名和输入密码时,从内存中比较密码,速度极快.
	QSqlQuery query;
    query.exec("select * from userInfo");

    while(query.next()){
        ui_.txtUserName->addItem(query.value(0).toString());
        //将对应用户的密码和类型存入两个链表,在用户登录时对比,比从数据库中查询速度要快.
		userPwd_ << query.value(1).toString();
        userType_ << query.value(2).toString();
    }

    //对应显示软件标题和工程商信息.
    ui_.labTitle->setText("Security System");
    ui_.labCompany->setText(QString("Technology support :%1").arg("scut"));
    ui_.txtUserName->setCurrentIndex(ui_.txtUserName->findText("admin"));    

    //增加软键盘处理
    ui_.gboxSoftKey->setGeometry(30,76,261,61);
    ui_.gboxSoftKey->setVisible(false);

	//优化 内存泄漏
    QList<QPushButton *> btn=ui_.gboxSoftKey->findChildren<QPushButton *>();//这行代码学到东西了呀，把软键盘下的children按钮组成一个List
    foreach (QPushButton *b, btn)
	{
        connect(b,SIGNAL(clicked()),this,SLOT(buttonClick()));
    }
}

//这个函数的意义在于用户可以键盘按键登陆，用到了事件
bool UserLogin::eventFilter(QObject *obj, QEvent *event)
{
	//用户按下回车键,触发登录信号.
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

//登陆按钮的槽函数，在这里new一个SecritySystem的窗口
void UserLogin::on_btnLogin_clicked()
{
	QString UserPwd = ui_.txtUserPwd->text();
	if (UserPwd=="")
	{
		Utility::ShowMessageBoxError("密码不能为空，请重新输入");
		ui_.txtUserPwd->setFocus();//将光标设在键盘输入密码处
		return;
	}

	int index=ui_.txtUserName->currentIndex();
	if (UserPwd == userPwd_[index])
	{
		Config::lastLoginUser = ui_.txtUserName->currentText();
		Config::currentUserName = Config::lastLoginUser;
		Config::currentUserPassword = UserPwd;
		Config::currentUserType = userType_[index];
		//优化 内存泄漏
		SecuritySystem *w = new SecuritySystem();
		w->show();
		this->hide();
	}else{
		Utility::ShowMessageBoxError("密码错误，请重新输入");
		ui_.txtUserPwd->setFocus();
	}
}

void UserLogin::on_btnClose_clicked()
{
	qApp->quit();
}

void UserLogin::buttonClick()
{
	QPushButton *btn=(QPushButton *)sender();//sender()找到哪个对象触发该槽函数
	QString objectName=btn->objectName();
	QString text=ui_.txtUserPwd->text();

	if (objectName=="btnDelete"){
		if (text.length()>0){
			ui_.txtUserPwd->setText(text.mid(0,text.length()-1));//Mid()函数也是很强大的：取0到length-1的字符串
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

//这个按钮就是小键盘，按下则弹出软键盘
void UserLogin::on_btnKey_clicked()
{
	ui_.gboxSoftKey->setVisible(!ui_.gboxSoftKey->isVisible());
}