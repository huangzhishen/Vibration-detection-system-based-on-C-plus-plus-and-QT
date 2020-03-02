#include "securitysystem.h"
#include "ThreadObject.h"
#include <QThread>
#include <qDebug>
#include <QSqlQuery>
#include <QTreeWidgetItem>
#include "DefenceSetting.h"
#include "HostSetting.h"
#include "SystemSetting.h"
#include "Config.h"
#include "DataQuery.h"


//reqObj_发送信号emitReceivedData(QVector<QByteArray>)，调用槽函数new_data_recieved(QVector<QByteArray>)，进而发出信号do_task，再线程池中pTaskThreadPool_触发SLOT(append_new(QObject *, const QByteArray &))
//定时器引发的信号emitGetData(QString,quint16)), 然后reqObj_调用槽函数SLOT(getData(QString,quint16))
//哦，原来在SecuritySystem的构造函数中解决了关于线程的代码
SecuritySystem::SecuritySystem(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags),isStart_(false),pdataProcess_(0),pTaskThreadPool_(0),pixmap_(":/image/notice.png")
{
	ui_.setupUi(this);
	ui_.mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);//文本显示在工具栏图标的旁边

	Utility::moveToCenter(this,Config::DeskWidth,Config::DeskHeigth);
	Utility::setWindowsMax(this);

	initForm();
	initTableWidget();
	initTreeWidget();
	initDefenceButton();
	setDefenceButtonVisible();


	for (int i = 0 ; i < 30 ; i++)//主界面左边的信息栏，一共有30行
	{
		itemTime_.push_back(std::make_shared<QTableWidgetItem>());
		itemArea_.push_back(std::make_shared<QTableWidgetItem>());
		itemMessage_.push_back(std::make_shared<QTableWidgetItem>());
		ui_.tableWidget->setItem(i,0,itemTime_[i].get());
		ui_.tableWidget->setItem(i,1,itemMessage_[i].get());
		ui_.tableWidget->setItem(i,2,itemArea_[i].get());
	}
	highPriTimer_ = nullptr;
	//int CountWarning[8];

	//创建用于收发UDP数据的线程
	thread_ = new QThread;
	reqObj_= new RequestDatagram();//请求数据的线程类
	reqObj_->moveToThread(thread_);

	qRegisterMetaType<QVector<QByteArray> > ("QVector<QByteArray>"); //该数据类型可在运行时动态创建和销毁该类型的对象。

	QObject::connect(this, SIGNAL(emitGetData(QString,quint16)), reqObj_, SLOT(getData(QString,quint16)));
	QObject::connect(reqObj_, SIGNAL(emitReceivedData(QVector<QByteArray>)), this, SLOT(new_data_recieved(QVector<QByteArray>)));
	thread_->start();

	/*串口通信*/
	struct PortSettings ComSetting = {BAUD9600,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,500}; //定义一个结构体，用来存放串口各个参数
	Com_ = new Win_QextSerialPort("com7", ComSetting,QextSerialBase::EventDriven);//定义串口对象，并传递参数，在构造函数里对其进行初始化
	Com_->open(QIODevice::ReadWrite);//以可读写方式打开串口(在构造函数中实现)
	connect(Com_,SIGNAL(readyRead()),this,SLOT(readComMessage()));//信号和槽函数关联，当串口缓冲区有数据时，进行读串口操作
	Com_->write(QString("AT+CNMI=1,1").append('\r').toAscii());
	
	mytime1 =new QTimer(this);
	mytime1->setInterval(60000);
	connect(mytime1,SIGNAL(timeout()),this,SLOT(AutoSend1()));
	mytime2 =new QTimer(this);
	mytime2->setInterval(60000);
	connect(mytime2,SIGNAL(timeout()),this,SLOT(AutoSend2()));
	mytime3 =new QTimer(this);
	mytime3->setInterval(60000);
	connect(mytime3,SIGNAL(timeout()),this,SLOT(AutoSend3()));
	mytime4 =new QTimer(this);
	mytime4->setInterval(60000);
	connect(mytime4,SIGNAL(timeout()),this,SLOT(AutoSend4()));
	mytime5 =new QTimer(this);
	mytime5->setInterval(60000);
	connect(mytime5,SIGNAL(timeout()),this,SLOT(AutoSend5()));
	mytime6 =new QTimer(this);
	mytime6->setInterval(60000);
	connect(mytime6,SIGNAL(timeout()),this,SLOT(AutoSend6()));
	mytime7 =new QTimer(this);
	mytime7->setInterval(60000);
	connect(mytime7,SIGNAL(timeout()),this,SLOT(AutoSend7()));
	mytime8 =new QTimer(this);
	mytime8->setInterval(60000);
	connect(mytime8,SIGNAL(timeout()),this,SLOT(AutoSend8()));

/*	mytimew1 =new QTimer(this);
	mytimew1->setInterval(120000);
	connect(mytimew1,SIGNAL(timeout()),this,SLOT(AutoSendw1()));
	mytimew2 =new QTimer(this);
	mytimew2->setInterval(120000);
	connect(mytimew2,SIGNAL(timeout()),this,SLOT(AutoSendw2()));
	mytimew3 =new QTimer(this);
	mytimew3->setInterval(120000);
	connect(mytimew3,SIGNAL(timeout()),this,SLOT(AutoSendw3()));
	mytimew4 =new QTimer(this);
	mytimew4->setInterval(120000);
	connect(mytimew4,SIGNAL(timeout()),this,SLOT(AutoSendw4()));
	mytimew5 =new QTimer(this);
	mytimew5->setInterval(120000);
	connect(mytimew5,SIGNAL(timeout()),this,SLOT(AutoSendw5()));
	mytimew6 =new QTimer(this);
	mytimew6->setInterval(120000);
	connect(mytimew6,SIGNAL(timeout()),this,SLOT(AutoSendw6()));
	mytimew7 =new QTimer(this);
	mytimew7->setInterval(120000);
	connect(mytimew7,SIGNAL(timeout()),this,SLOT(AutoSendw7()));
	mytimew8 =new QTimer(this);
	mytimew8->setInterval(120000);
	connect(mytimew8,SIGNAL(timeout()),this,SLOT(AutoSendw8()));
*/
	
}

SecuritySystem::~SecuritySystem()
{
	if (highPriTimer_)
	{
		highPriTimer_->stop();
		disconnect(highPriTimer_,SIGNAL(timeout()),this,SLOT(sendMsgtoServer()));
		highPriTimer_->deleteLater();
		highPriTimer_ = nullptr;
	}
	thread_->exit(0);
	if (reqObj_)
	{
		reqObj_->deleteLater();
	}
	thread_->deleteLater();
}

//主界面的状态栏信息，设置定时器记录软件当前已运行时间
void SecuritySystem::initForm()
{
	setWindowTitle(Config::softwareTitle);
	//状态栏
	//优化 内存泄漏
	labWelcome_=new QLabel(tr("欢迎使用 %1").arg(Config::softwareTitle));
	ui_.statusBar->addWidget(labWelcome_);
	labUser_ =new QLabel(tr("      当前用户:%1[%2]      ").arg(Config::currentUserName).arg(Config::currentUserType));
	ui_.statusBar->addWidget(labUser_);
	labLive_=new QLabel(tr("      已运行:0天0时0分0秒      "));
	ui_.statusBar->addWidget(labLive_);
	labTime_=new QLabel(QDateTime::currentDateTime().toString(tr("当前时间: ") +"yyyy " + tr("年") + "MM " + tr("月")+"dd "+tr("日") + " dddd "+" HH:mm:ss"));
	ui_.statusBar->addWidget(labTime_);

	timerDate_ =new QTimer(this);
	timerDate_->setInterval(1000);
	connect(timerDate_,SIGNAL(timeout()),this,SLOT(showDateTime()));
	timerDate_->start();
}

//一秒更新一次，利用定时器，定时引发槽函数。这个运行时间的累计函数值得学习！
void SecuritySystem::showDateTime()
{
	static int day=0;//静态变量只初始化一次
	static int hour=0;
	static int minute=0;
	static int second=0;
	second++;
	if (second==60){minute++;second=0;}
	if (minute==60){hour++;minute=0;}
	if (hour==24){day++;hour=0;}

	//这两行代码之所以在写一次，是因为时间需要每时每刻改变，所以需要写在定时器的槽函数，好！
	labLive_->setText(tr("      已运行:%1天%2时%3分%4秒      ").arg(day).arg(hour).arg(minute).arg(second));
	labTime_->setText(QDateTime::currentDateTime().toString(tr("当前时间: ") +"yyyy " + tr("年") + "MM " + tr("月")+"dd "+tr("日") + " dddd "+" HH:mm:ss"));
}

//主界面中的警情界面
void SecuritySystem::initTableWidget()
{
	//警情
	ui_.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui_.tableWidget->verticalHeader()->setVisible(false);
	ui_.tableWidget->setColumnCount(3);
	ui_.tableWidget->setColumnWidth(0,150);
	ui_.tableWidget->setColumnWidth(1,75);
	ui_.tableWidget->setColumnWidth(2,75);
	int count=30;
	ui_.tableWidget->setRowCount(count);

	QStringList headText;
	headText<<tr("时间")<<tr("事件")<<tr("防区");
	ui_.tableWidget->setHorizontalHeaderLabels(headText);//设置表头
	ui_.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许双击编辑
	ui_.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//选中模式为单行选中
	ui_.tableWidget->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
	ui_.tableWidget->horizontalHeader()->setResizeMode(1,QHeaderView::Fixed);
	ui_.tableWidget->horizontalHeader()->setResizeMode(2,QHeaderView::Fixed);
	ui_.tableWidget->verticalHeader()->setResizeMode(QHeaderView::Stretch);
}

//主界面的主机tree界面
void SecuritySystem::initTreeWidget()
{
	//主机
	ui_.treeWidget->setColumnCount(1); //设置列数
	ui_.treeWidget->setColumnWidth(0,250);
	ui_.treeWidget->header()->setVisible(false);//隐藏列标题
	ui_.treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不能编辑

	ui_.treeWidget->clear();
	QSqlQuery query;
	QString sql = "select hostID, hostName from hostInfo";      
	query.exec(sql);
	while (query.next())
	{
		QString hostID = query.value(0).toString();
		QString hostName = query.value(1).toString();
		//优化 内存泄漏
		QTreeWidgetItem *hostItem = new QTreeWidgetItem(ui_.treeWidget,QStringList("["+hostID+"]"+hostName));
		hostItem->setIcon(0,QIcon(":/image/tree1.png"));

		QSqlQuery defenceQuery;
		QString defenceSql = "select * from defenceInfo where hostID='" + hostID + "'";
		defenceQuery.exec(defenceSql);
		while (defenceQuery.next())
		{
			QString defenceID = defenceQuery.value(0).toString();
			defenceMap_ << defenceQuery.value(4).toString();
			//优化 内存泄漏
			QTreeWidgetItem *defenceItem = new QTreeWidgetItem(hostItem,QStringList("Area."+defenceID));
			defenceItem->setIcon(0,QIcon(":/image/tree2.png"));
			hostItem->addChild(defenceItem);
		}
	}
	ui_.treeWidget->expandAll();
	defenceMap_.removeDuplicates();
	if (defenceMap_.size())
	{
		Config::CurrentImage = defenceMap_.at(0);
	}
}

//可优化, 与initTreeWidget合并减少数据库访问时间，这个防区按钮初始化也是干了很多活，基本上都是数据库查询防区数据，然后在界面显示。
void SecuritySystem::initDefenceButton()//初始化防区设置
{
	qDeleteAll(ui_.boxMain->findChildren<DefenceButton*>());//删除界面上所有按钮对象，然后重新建立，相当于初始化清零
	QSqlQuery query;
	QString sql = "select * from defenceInfo";
	query.exec(sql);
	while(query.next())
	{
		QString defenceID = query.value(0).toString();
		int defenceType = query.value(1).toInt();
		QString hostID = query.value(2).toString();
		QString hostName = query.value(3).toString();
		QString defenceMap = query.value(4).toString();
		QString defenceSound = query.value(5).toString();
		//w 10,1580    h 15,850
		int defencePosX = Config::defenceOffsetX + query.value(6).toFloat() * Config::defenceBGWidth;
		int defencePosY = Config::defenceOffsetY + query.value(7).toFloat() * Config::defenceBGHeight;

		//优化 内存泄漏
		DefenceButton *btn = new DefenceButton(ui_.boxMain, defenceID, Utility::getDefenceType(defenceType), hostID, hostName,defenceSound,defenceMap,Config::currentUserName, defencePosX, defencePosY, DefenceStatus_init);

		QSqlQuery queryHostInfo;
		queryHostInfo.exec("select hostIP,hostPort from hostinfo where hostID="+hostID);
		while(queryHostInfo.next())
		{
			QString IP = queryHostInfo.value(0).toString();
			int Port = queryHostInfo.value(1).toInt();
			btn->setHostAddrAndPort(IP,Port);//从数据库中提取IP与端口
		}

		btn->setGeometry(defencePosX,defencePosY, Config::defenceSize,Config::defenceSize);
		btn->setVisible(true);//显示
		int iDefenceID = defenceID.toInt();
		clientID_.insert(iDefenceID);
		mapDefenceBtn_[iDefenceID] = btn;

		connect(btn,SIGNAL(sig_resetList(int)),this,SLOT(resetNotice(int)));

		QLabel *noticeLabel = new QLabel(this);
		mapDefenceLabel_[iDefenceID] = noticeLabel;
		noticeLabel->setAutoFillBackground(true);
		noticeLabel->setVisible(false);
	}
}

//全部启动监听，设置一个选项(要么单地图，要么多地图)，一旦启动监听就不能切换//除了查询按钮，其他按钮禁止
//当点击按钮时，设置定时器，定时查询消息，两个connect连接完成以下任务：
//接受来自线程池的任务完成信号done_task(QObject *)，用process_finished(QObject *)处理；
//do_task(QObject *, const QByteArray &)信号来了以后，线程池触发append_new(QObject *, const QByteArray &)
//定时器类，时间到了，就调用sendMsgtoServer()，发出信号emitGetData（iter.value()->getHostAddr(),iter.value()->getHostPort()）
void SecuritySystem::on_btnStartMoniter_clicked()//启动监听按钮
{
	if (isStart_==false)
	{
		//优化 内存泄漏
		pdataProcess_ = new DataProcess(this);

		pTaskThreadPool_ = new TaskThreadPool(this,pdataProcess_,Config::defenceCount);

		connect(pTaskThreadPool_,SIGNAL(done_task(QObject *)),this,SLOT(process_finished(QObject *)));

		connect(this,SIGNAL(do_task(QObject *, const QByteArray &)),pTaskThreadPool_,SLOT(append_new(QObject *, const QByteArray &)));

		isStart_ = true;
		ui_.systemAction->setDisabled(true);//启动监控后，设置这三个按钮为不可用
		ui_.hostManageAction->setDisabled(true);
		ui_.defenceAction->setDisabled(true);
		ui_.btnStartMoniter->setText(tr("停止监控"));
		Utility::isMainWidget = true;

		//这个关键：创建一个用户询问，记录这个询问的时间
		//赋值tcpclients这个map(clienquery,qdatatime)
		if (tcpClients_.size() < clientID_.size())//tcpclients_是clientquery对应的时间，clientID是对应一个数组
		{
			foreach (int i,clientID_)//遍历这一个哈希表，clientID是Qset，i是值
			{
				//原来需要把i传进去，clientquery类中这个参数是代表
				ClientQuery * client = new ClientQuery(this,i,mapDefenceBtn_[i],mapDefenceBtn_[i]->getHostAddr(), mapDefenceBtn_[i]->getHostPort());
				tcpClients_[client] = QDateTime::currentDateTime();
				mapDefenceBtn_[i]->SetDefenceStatus(DefenceStatus_normal);
			}
		}
		
		//自己创建了一定时器类，时间到了，就调用sendMsgtoServer()
		if (highPriTimer_ == nullptr)
		{
			highPriTimer_ = new mmtimer(Config::timeGap,this);
			connect(highPriTimer_,SIGNAL(timeout()),this,SLOT(sendMsgtoServer()));
		}
		highPriTimer_->start();
	}
	else//哦，这里是用一个bool变量判断如果监控没开，则开启，建立线程池，建立数据处理线程；否则断开信号连接，计时停止。
	{
		/*killTimer(Timer_);*/
		QList<ClientQuery*> listObj = tcpClients_.keys();//键值对key-value
		foreach(ClientQuery * sock,listObj)
		{
			mapDefenceBtn_[sock->getDefenceID().toInt()]->SetDefenceStatus(DefenceStatus_init);
			sock->close();
		}
		//优化 内存泄漏
		tcpClients_.clear();
		//断开连接：线程池完成任务的信号、本窗口新任务的信号
		disconnect(pTaskThreadPool_,SIGNAL(done_task(QObject *)),this,SLOT(process_finished(QObject *)));
		disconnect(this,SIGNAL(do_task(QObject *, const QByteArray &)),pTaskThreadPool_,SLOT(append_new(QObject *, const QByteArray &)));
		//所有定时器停止
		if (highPriTimer_)
		{
			highPriTimer_->stop();
		}
		//这里是师兄加的，即停止监控时，停止定时发送
		if (mytime1->isActive())
		{
			mytime1->stop();
		}
		if (mytime2->isActive())
		{
			mytime2->stop();
		}
		if (mytime3->isActive())
		{
			mytime3->stop();
		}
		if (mytime4->isActive())
		{
			mytime4->stop();
		}
		if (mytime5->isActive())
		{
			mytime5->stop();
		}
		if (mytime6->isActive())
		{
			mytime6->stop();
		}
		if (mytime7->isActive())
		{
			mytime7->stop();
		}
		if (mytime8->isActive())
		{
			mytime8->stop();
		}

		//优化 内存泄漏
		//bug, smart pointer
		//pdataProcess_->deleteLater();
		//pTaskThreadPool_->deleteLater();

		isStart_ = false;
		ui_.systemAction->setEnabled(true);
		ui_.hostManageAction->setEnabled(true);
		ui_.defenceAction->setEnabled(true);
		ui_.btnStartMoniter->setText(tr("启动监控"));

		Utility::isMainWidget = false;
	}
}

//四个按钮的槽函数，实现都很简单，建立对应窗口对象，运行，但是有对象释放的问题，每次点击都要新建对象
void SecuritySystem::on_systemAction_triggered()//系统设置
{
	//优化 内存泄漏
	SystemSetting *sys = new SystemSetting;
//	sys->setAttribute(Qt::WA_DeleteOnClose);
	sys->exec();
}
void SecuritySystem::on_hostManageAction_triggered()//主机管理
{
	//优化 内存泄漏
	HostSetting *host = new HostSetting;
//	host->setAttribute(Qt::WA_DeleteOnClose);
	host->exec();
}
void SecuritySystem::on_defenceAction_triggered()//防区设置
{
	//优化 内存泄漏
	DefenceSetting *defence=new DefenceSetting;
//	defence->setAttribute(Qt::WA_DeleteOnClose);
	defence->exec();
}
void SecuritySystem::on_queryAction_triggered()//警情查询
{
	DataQuery *w = new DataQuery;
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
}

bool isSend = false;

bool isdanger1 = false;
bool isdanger2 = false;
bool isdanger3 = false;
bool isdanger4 = false;
bool isdanger5 = false;
bool isdanger6 = false;
bool isdanger7 = false;
bool isdanger8 = false;

/*
bool iswarn1 = false;
bool iswarn2 = false;
bool iswarn3 = false;
bool iswarn4 = false;
bool iswarn5 = false;
bool iswarn6 = false;
bool iswarn7 = false;
bool iswarn8 = false;
*/

//线程池发出任务完成信号done_task(QObject *)，处理函数：左侧30行数据开始生成，地图中警告危险信息也生成
void SecuritySystem::process_finished(QObject *obj)
{
	static int messageCount = 0;
	//线程安全问题
 	ClientQuery *cli = static_cast<ClientQuery*>(obj);//参数转换了一下类型
	if (cli->isEventHappen())//警告，危险事件发生
	{
		if (messageCount >= 30 )//大于30，即满格，需要生成新的30格
		{
			//ui_.tableWidget->clearContents();
			for (int i = 0; i < 30 ; i++)
			{
				itemTime_[i]->setText("");
				itemArea_[i]->setText("");
				itemMessage_[i]->setText("");
			}
			messageCount=0;
		}
		itemTime_[messageCount]->setText(cli->getEventTime().right(8));//记录该事件的时间
		itemArea_[messageCount]->setText(QString(cli->getDefenceID()));//记录该事件发生的区域

		if (cli->isEventDanger())//在发生事件的基础上，再判断具体什么事件，危险或警告，这里可以大大的优化
		{
			itemMessage_[messageCount]->setText(tr("危险"));
			switch (cli->getDefenceID().toInt())
			{ 
			case 1 :
				//下面实现发送警报信息给手机
				if(isdanger1==false)
				{	
					isdanger1=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区1发生入侵事件，级别：危险")).toAscii());
							}
						}
					}
					mytime1->start();	
				}
				break; 
			case 2 :
				if(isdanger2==false)
				{	
					//下面实现发送警报信息给手机
					isdanger2=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区2发生入侵事件，级别：危险")).toAscii());
							}
						}
					}
					mytime2->start();
				}break; 
			case 3 :
				if(isdanger3==false)
				{	
					//下面实现发送警报信息给手机
					isdanger3=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区3发生入侵事件，级别：危险")).toAscii());
							}
						}
					}
					mytime3->start();
				} break;
			case 4 :
				if(isdanger4==false)
				{	
					//下面实现发送警报信息给手机
					isdanger4=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区4发生入侵事件，级别：危险")).toAscii());
							}
						}
					}
					mytime4->start();
				} break; 
			case 5 : 
				if(isdanger5==false)
				{	
					//下面实现发送警报信息给手机
					isdanger5=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区5发生入侵事件，级别：危险")).toAscii());
							}
						}
					}
					mytime5->start();
				} break;
			case 6 : 
				if(isdanger6==false)
				{	
					//下面实现发送警报信息给手机
					isdanger6=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区6发生入侵事件，级别：危险")).toAscii());
							}
						}
					}
					mytime6->start();
				}  break; 
			case 7 :  
				if(isdanger7==false)
				{	
					//下面实现发送警报信息给手机
					isdanger7=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区7发生入侵事件，级别：危险")).toAscii());
							}
						}
					}
					mytime7->start();
				}  break; 
			case 8 :
				if(isdanger8==false)
				{	
					//下面实现发送警报信息给手机
					isdanger8=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								//Com_->write(QString("AT+CMGS=\"").append(stringToUnicode(tr("18716035785"))).append("\"").append("\r").toAscii());
								Com_->write(stringToUnicode(tr("防区8发生入侵事件，级别：危险")).toAscii());
							}
						}
					}
					mytime8->start();
				}  break;
			}
		}
		else
		{
			itemMessage_[messageCount]->setText(tr("警告"));
			/*
			switch (cli->getDefenceID().toInt())
			{ 
			case 1 :
				//下面实现发送警报信息给手机
				//
				if(iswarn1==false)
				{	
					iswarn1=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区1发生入侵事件，级别：警告")).toAscii());
							}
						}
					}
					
					mytimew1->start();

				}break; 

			case 2 :
				if(iswarn2==false)
				{	
					//下面实现发送警报信息给手机
					iswarn2=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区2发生入侵事件，级别：警告")).toAscii());
							}
						}
					}
					mytimew2->start();

				}break; 
			case 3 :
				if(iswarn3==false)
				{	
					//下面实现发送警报信息给手机
					iswarn3=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区3发生入侵事件，级别：警告")).toAscii());
							}
						}
					}
					mytimew3->start();
				} break; 
			case 4 :
				if(iswarn4==false)
				{	
					//下面实现发送警报信息给手机
					iswarn4=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区4发生入侵事件，级别：警告")).toAscii());
							}
						}
					}
					mytimew4->start();
				}  break; 
			case 5 : 
				if(iswarn5==false)
				{	
					//下面实现发送警报信息给手机
					iswarn5=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区5发生入侵事件，级别：警告")).toAscii());
							}
						}
					}
					mytimew5->start();
				} break;
			case 6 : 
				if(iswarn6==false)
				{	
					//下面实现发送警报信息给手机
					iswarn6=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区6发生入侵事件，级别：警告")).toAscii());
							}
						}
					}
					mytimew6->start();

				}  break; 
			case 7 :  
				if(iswarn7==false)
				{	
					//下面实现发送警报信息给手机
					iswarn7=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区7发生入侵事件，级别：警告")).toAscii());
							}
						}
					}
					mytimew7->start();
				}  break; 
			case 8 :
				if(iswarn8==false)
				{	
					//下面实现发送警报信息给手机
					iswarn8=true;
					if (isSend == false)//判断是否正在发送，没有正在发送则进入
					{
						isSend = true;//进入后，立即关闭入口
						if (Com_)
						{
							if (Com_->isOpen())//如果串口打开了
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("防区8发生入侵事件，级别：警告")).toAscii());
							}
						}
					}
					mytimew8->start();
				} break; 
			}
			*/
	     }
		messageCount++;
		int defenceID = cli->getDefenceID().toInt();
		DefenceButton *btn = mapDefenceBtn_[defenceID];
		int warnCount = btn->getWarningCount();
		int dangerCount = btn->getDangerCount();

		QLabel *noticeLabel = mapDefenceLabel_[defenceID];//用ID映射到对应的显示标签
		noticeLabel->setText(tr("__%1个警告，%2个危险_").arg(warnCount).arg(dangerCount));//地图中显示的标签
		int len = noticeLabel->text().size();//
		noticeLabel->setGeometry(btn->GetDefenceX()+Config::labelOffsetX,btn->GetDefenceY()+Config	::labelOffsetY, Config::labelWidth,Config::labelHeight);//让标签更好的显示
		pixmap_ = pixmap_.scaled(noticeLabel->size());//放大
		palette_.setBrush(QPalette::Window, QBrush(pixmap_));	//调色板
		noticeLabel->setMask(pixmap_.mask());//实现不规则窗体
		noticeLabel->setPalette(palette_);//
		noticeLabel->setVisible(true);//实现可见		
	}
}


void SecuritySystem::resetNotice(int ID)
{
	//优化 内存泄漏
	QLabel *noticeLabel = mapDefenceLabel_[ID];
	noticeLabel->setVisible(false);//重置则是使警报信息标签不可见，不可见之后，
	                               //再有信号数据来，就重新调上面的处理函数，其中的显示标签是新new的，应该不会有内存问题，
	                               //应该会及时释放！
}

//发出信号emitGetData
void SecuritySystem::sendMsgtoServer()
{
	if (mapDefenceBtn_.size())
	{
		auto iter = mapDefenceBtn_.begin();
		emit emitGetData(iter.value()->getHostAddr(),iter.value()->getHostPort());
	    //	sendCount++;
	}	
}

 void SecuritySystem::timerEvent(QTimerEvent * evt)
{
}

 //收到数据，发送信号do_task，然后，线程池中pTaskThreadPool_触发SLOT(append_new(QObject *, const QByteArray &))
 void SecuritySystem::new_data_recieved(QVector<QByteArray> data)
 {
	 QList<ClientQuery*> listObj = tcpClients_.keys();//tcpclients的key是clientsquery
	 foreach(ClientQuery * sock,listObj)
	 {
		 emit do_task(sock,data[sock->getDefenceID().toInt()-1]);//sock是clientsquery，data类似一个二维数组
	 }                                                           //这里是个循环，发出8个信号
 }

 //错误信息显示
void SecuritySystem::display_error(QAbstractSocket::SocketError err)
 {
	 ClientQuery * sock = qobject_cast<ClientQuery *> (sender());
	 if (sock)
		 qDebug() << QString("client %1 error msg:").arg((quintptr)sock)+sock->errorString();
 }

 

void SecuritySystem::setDefenceButtonVisible()
{
	ui_.boxMain->setStyleSheet(QString("border-image:url(%1MAP/%2);")
		.arg(Config::AppPath)
		.arg(Config::CurrentImage));
	QList<DefenceButton *> btn=ui_.boxMain->findChildren<DefenceButton *>();
	foreach (DefenceButton *b, btn){
		if (b->GetDefenceImage()==Config::CurrentImage){
			b->setVisible(true);
		}else{
			b->setVisible(false);
		}
	}
}

void SecuritySystem::on_cbRecordAll_stateChanged(int state)//记录全部数据那个勾选项
{
	Config::isRecordAllData = (state != 0);
	qDebug() << Config::isRecordAllData;
}
void SecuritySystem::on_treeWidget_currentItemChanged( QTreeWidgetItem *current, QTreeWidgetItem *previous)
 {
	 if (current == 0 || current->parent() == 0)
	 {
		 return ;
	 }
	 QString defenceID = current->text(0).split(".")[1];
	 QList<DefenceButton *> btn=ui_.boxMain->findChildren<DefenceButton *>();
	 foreach (DefenceButton *b, btn) {
		 if (b->GetDefenceID() == defenceID)
		 {
			 Config::CurrentImage = b->GetDefenceImage();
			 break;
		 }
	 }
	 setDefenceButtonVisible();
 }

void SecuritySystem::readComMessage() //读串口函数
{
	QByteArray temp = Com_->readAll();//读取串口缓冲区的所有数据给临时变量temp
	if(temp.indexOf('>')>=0)
	{
		char endCmd[1]={0X1A};
		Com_->write(endCmd);
		qDebug()<<" 444444444444"<<QDateTime ::currentDateTime().toString("hh:mm:ss.zzz");
		isSend=false;
		Com_->write(QString("AT+CNMI=1,1").append('\r').toAscii());
	}

	//收到信息位置
	if(temp.indexOf("+CMTI:")>=0)
	{
		QString bytes(temp);
		QStringList strs = bytes.split(',');
		Com_->write(QString("AT+CMGR=%1").arg(strs[1]).append('\r').toAscii());		
	}
	//读取到具体的信息
	if (temp.indexOf("006F006B0031") >= 0 )
	{
		//at+cmgd=1
		QString bytes(temp);
		int startPos = bytes.indexOf('=');
		int stopPos = bytes.indexOf('+');
		int len = stopPos - startPos-1;
		QString num = bytes.left(stopPos).right(len);
		Com_->write(QString("AT+CMGD=%1").arg(num).append('\r').toAscii());
		//time stop
		mytime1->stop();
		isdanger1=false;
	}
	else if(temp.indexOf("006F006B0032") >= 0 )
	{
		QString bytes(temp);
		int startPos = bytes.indexOf('=');
		int stopPos = bytes.indexOf('+');
		int len = stopPos - startPos-1;
		QString num = bytes.left(stopPos).right(len);
		Com_->write(QString("AT+CMGD=%1").arg(num).append('\r').toAscii());
		mytime2->stop();
		isdanger2=false;
	}
	else if(temp.indexOf("006F006B0033") >= 0 )
	{
		QString bytes(temp);
		int startPos = bytes.indexOf('=');
		int stopPos = bytes.indexOf('+');
		int len = stopPos - startPos-1;
		QString num = bytes.left(stopPos).right(len);
		Com_->write(QString("AT+CMGD=%1").arg(num).append('\r').toAscii());
		mytime3->stop();
		isdanger3=false;
	}
	else if(temp.indexOf("006F006B0034") >= 0 )
	{
		QString bytes(temp);
		int startPos = bytes.indexOf('=');
		int stopPos = bytes.indexOf('+');
		int len = stopPos - startPos-1;
		QString num = bytes.left(stopPos).right(len);
		Com_->write(QString("AT+CMGD=%1").arg(num).append('\r').toAscii());
		mytime4->stop();
		isdanger4=false;
	}
	else if(temp.indexOf("006F006B0035") >= 0 )
	{
		QString bytes(temp);
		int startPos = bytes.indexOf('=');
		int stopPos = bytes.indexOf("+CMGR:");
		int len = stopPos - startPos-1;
		QString num = bytes.left(stopPos).right(len);
		Com_->write(QString("AT+CMGD=%1").arg(num).append('\r').toAscii());
		mytime5->stop();
		isdanger5=false;
	}
	else if(temp.indexOf("006F006B0036") >= 0 )
	{
		QString bytes(temp);
		int startPos = bytes.indexOf('=');
		int stopPos = bytes.indexOf('+');
		int len = stopPos - startPos-1;
		QString num = bytes.left(stopPos).right(len);
		Com_->write(QString("AT+CMGD=%1").arg(num).append('\r').toAscii());
		mytime6->stop();
		isdanger6=false;
	}
	else if(temp.indexOf("006F006B0037") >= 0 )
	{
		QString bytes(temp);
		int startPos = bytes.indexOf('=');
		int stopPos = bytes.indexOf('+');
		int len = stopPos - startPos-1;
		QString num = bytes.left(stopPos).right(len);
		Com_->write(QString("AT+CMGD=%1").arg(num).append('\r').toAscii());
		mytime7->stop();
		isdanger7=false;
	}
	else if(temp.indexOf("006F006B0038") >= 0 )
	{
		QString bytes(temp);
		int startPos = bytes.indexOf('=');
		int stopPos = bytes.indexOf('+');
		int len = stopPos - startPos-1;
		QString num = bytes.left(stopPos).right(len);
		Com_->write(QString("AT+CMGD=%1").arg(num).append('\r').toAscii());
		mytime8->stop();
		isdanger8=false;
	}
}
QString SecuritySystem::stringToUnicode(QString str)
{
	const QChar *q;
	QChar qtmp;
	QString str0;
	QString strout;
	int num;
	q=str.unicode();
	int len=str.count();
	for(int i=0;i<len;i++)
	{   
		qtmp =(QChar)*q++;
		num= qtmp.unicode();
		if(num<255)
			strout+="00"; //英文或数字前加"00"
		str0=str0.setNum(num,16);//变成十六进制数
		strout+=str0;
	}
	return strout;
}



void SecuritySystem::AutoSend1()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区1发生入侵事件，级别：危险")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend2()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区2发生入侵事件，级别：危险")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend3()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区3发生入侵事件，级别：危险")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend4()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区4发生入侵事件，级别：危险")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend5()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区5发生入侵事件，级别：危险")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend6()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区6发生入侵事件，级别：危险")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend7()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区7发生入侵事件，级别：危险")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend8()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区8发生入侵事件，级别：危险")).toAscii());
			}
		}
	}
}


/*
void SecuritySystem::AutoSendw1()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	
				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区1发生入侵事件，级别：警告")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw2()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区2发生入侵事件，级别：警告")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw3()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区3发生入侵事件，级别：警告")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw4()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区4发生入侵事件，级别：警告")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw5()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区5发生入侵事件，级别：警告")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw6()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区6发生入侵事件，级别：警告")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw7()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区7发生入侵事件，级别：警告")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw8()
{
	//下面实现发送警报信息给手机
	if (isSend == false)//判断是否正在发送，没有正在发送则进入
	{
		isSend = true;//进入后，立即关闭入口
		if (Com_)
		{
			if (Com_->isOpen())//如果串口打开了
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("防区8发生入侵事件，级别：警告")).toAscii());
			}
		}
	}
}

*/


		