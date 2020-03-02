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


//reqObj_�����ź�emitReceivedData(QVector<QByteArray>)�����òۺ���new_data_recieved(QVector<QByteArray>)�����������ź�do_task�����̳߳���pTaskThreadPool_����SLOT(append_new(QObject *, const QByteArray &))
//��ʱ���������ź�emitGetData(QString,quint16)), Ȼ��reqObj_���òۺ���SLOT(getData(QString,quint16))
//Ŷ��ԭ����SecuritySystem�Ĺ��캯���н���˹����̵߳Ĵ���
SecuritySystem::SecuritySystem(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags),isStart_(false),pdataProcess_(0),pTaskThreadPool_(0),pixmap_(":/image/notice.png")
{
	ui_.setupUi(this);
	ui_.mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);//�ı���ʾ�ڹ�����ͼ����Ա�

	Utility::moveToCenter(this,Config::DeskWidth,Config::DeskHeigth);
	Utility::setWindowsMax(this);

	initForm();
	initTableWidget();
	initTreeWidget();
	initDefenceButton();
	setDefenceButtonVisible();


	for (int i = 0 ; i < 30 ; i++)//��������ߵ���Ϣ����һ����30��
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

	//���������շ�UDP���ݵ��߳�
	thread_ = new QThread;
	reqObj_= new RequestDatagram();//�������ݵ��߳���
	reqObj_->moveToThread(thread_);

	qRegisterMetaType<QVector<QByteArray> > ("QVector<QByteArray>"); //���������Ϳ�������ʱ��̬���������ٸ����͵Ķ���

	QObject::connect(this, SIGNAL(emitGetData(QString,quint16)), reqObj_, SLOT(getData(QString,quint16)));
	QObject::connect(reqObj_, SIGNAL(emitReceivedData(QVector<QByteArray>)), this, SLOT(new_data_recieved(QVector<QByteArray>)));
	thread_->start();

	/*����ͨ��*/
	struct PortSettings ComSetting = {BAUD9600,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,500}; //����һ���ṹ�壬������Ŵ��ڸ�������
	Com_ = new Win_QextSerialPort("com7", ComSetting,QextSerialBase::EventDriven);//���崮�ڶ��󣬲����ݲ������ڹ��캯���������г�ʼ��
	Com_->open(QIODevice::ReadWrite);//�Կɶ�д��ʽ�򿪴���(�ڹ��캯����ʵ��)
	connect(Com_,SIGNAL(readyRead()),this,SLOT(readComMessage()));//�źźͲۺ��������������ڻ�����������ʱ�����ж����ڲ���
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

//�������״̬����Ϣ�����ö�ʱ����¼�����ǰ������ʱ��
void SecuritySystem::initForm()
{
	setWindowTitle(Config::softwareTitle);
	//״̬��
	//�Ż� �ڴ�й©
	labWelcome_=new QLabel(tr("��ӭʹ�� %1").arg(Config::softwareTitle));
	ui_.statusBar->addWidget(labWelcome_);
	labUser_ =new QLabel(tr("      ��ǰ�û�:%1[%2]      ").arg(Config::currentUserName).arg(Config::currentUserType));
	ui_.statusBar->addWidget(labUser_);
	labLive_=new QLabel(tr("      ������:0��0ʱ0��0��      "));
	ui_.statusBar->addWidget(labLive_);
	labTime_=new QLabel(QDateTime::currentDateTime().toString(tr("��ǰʱ��: ") +"yyyy " + tr("��") + "MM " + tr("��")+"dd "+tr("��") + " dddd "+" HH:mm:ss"));
	ui_.statusBar->addWidget(labTime_);

	timerDate_ =new QTimer(this);
	timerDate_->setInterval(1000);
	connect(timerDate_,SIGNAL(timeout()),this,SLOT(showDateTime()));
	timerDate_->start();
}

//һ�����һ�Σ����ö�ʱ������ʱ�����ۺ������������ʱ����ۼƺ���ֵ��ѧϰ��
void SecuritySystem::showDateTime()
{
	static int day=0;//��̬����ֻ��ʼ��һ��
	static int hour=0;
	static int minute=0;
	static int second=0;
	second++;
	if (second==60){minute++;second=0;}
	if (minute==60){hour++;minute=0;}
	if (hour==24){day++;hour=0;}

	//�����д���֮������дһ�Σ�����Ϊʱ����Ҫÿʱÿ�̸ı䣬������Ҫд�ڶ�ʱ���Ĳۺ������ã�
	labLive_->setText(tr("      ������:%1��%2ʱ%3��%4��      ").arg(day).arg(hour).arg(minute).arg(second));
	labTime_->setText(QDateTime::currentDateTime().toString(tr("��ǰʱ��: ") +"yyyy " + tr("��") + "MM " + tr("��")+"dd "+tr("��") + " dddd "+" HH:mm:ss"));
}

//�������еľ������
void SecuritySystem::initTableWidget()
{
	//����
	ui_.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui_.tableWidget->verticalHeader()->setVisible(false);
	ui_.tableWidget->setColumnCount(3);
	ui_.tableWidget->setColumnWidth(0,150);
	ui_.tableWidget->setColumnWidth(1,75);
	ui_.tableWidget->setColumnWidth(2,75);
	int count=30;
	ui_.tableWidget->setRowCount(count);

	QStringList headText;
	headText<<tr("ʱ��")<<tr("�¼�")<<tr("����");
	ui_.tableWidget->setHorizontalHeaderLabels(headText);//���ñ�ͷ
	ui_.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//������˫���༭
	ui_.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//ѡ��ģʽΪ����ѡ��
	ui_.tableWidget->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
	ui_.tableWidget->horizontalHeader()->setResizeMode(1,QHeaderView::Fixed);
	ui_.tableWidget->horizontalHeader()->setResizeMode(2,QHeaderView::Fixed);
	ui_.tableWidget->verticalHeader()->setResizeMode(QHeaderView::Stretch);
}

//�����������tree����
void SecuritySystem::initTreeWidget()
{
	//����
	ui_.treeWidget->setColumnCount(1); //��������
	ui_.treeWidget->setColumnWidth(0,250);
	ui_.treeWidget->header()->setVisible(false);//�����б���
	ui_.treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//���ܱ༭

	ui_.treeWidget->clear();
	QSqlQuery query;
	QString sql = "select hostID, hostName from hostInfo";      
	query.exec(sql);
	while (query.next())
	{
		QString hostID = query.value(0).toString();
		QString hostName = query.value(1).toString();
		//�Ż� �ڴ�й©
		QTreeWidgetItem *hostItem = new QTreeWidgetItem(ui_.treeWidget,QStringList("["+hostID+"]"+hostName));
		hostItem->setIcon(0,QIcon(":/image/tree1.png"));

		QSqlQuery defenceQuery;
		QString defenceSql = "select * from defenceInfo where hostID='" + hostID + "'";
		defenceQuery.exec(defenceSql);
		while (defenceQuery.next())
		{
			QString defenceID = defenceQuery.value(0).toString();
			defenceMap_ << defenceQuery.value(4).toString();
			//�Ż� �ڴ�й©
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

//���Ż�, ��initTreeWidget�ϲ��������ݿ����ʱ�䣬���������ť��ʼ��Ҳ�Ǹ��˺ܶ������϶������ݿ��ѯ�������ݣ�Ȼ���ڽ�����ʾ��
void SecuritySystem::initDefenceButton()//��ʼ����������
{
	qDeleteAll(ui_.boxMain->findChildren<DefenceButton*>());//ɾ�����������а�ť����Ȼ�����½������൱�ڳ�ʼ������
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

		//�Ż� �ڴ�й©
		DefenceButton *btn = new DefenceButton(ui_.boxMain, defenceID, Utility::getDefenceType(defenceType), hostID, hostName,defenceSound,defenceMap,Config::currentUserName, defencePosX, defencePosY, DefenceStatus_init);

		QSqlQuery queryHostInfo;
		queryHostInfo.exec("select hostIP,hostPort from hostinfo where hostID="+hostID);
		while(queryHostInfo.next())
		{
			QString IP = queryHostInfo.value(0).toString();
			int Port = queryHostInfo.value(1).toInt();
			btn->setHostAddrAndPort(IP,Port);//�����ݿ�����ȡIP��˿�
		}

		btn->setGeometry(defencePosX,defencePosY, Config::defenceSize,Config::defenceSize);
		btn->setVisible(true);//��ʾ
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

//ȫ����������������һ��ѡ��(Ҫô����ͼ��Ҫô���ͼ)��һ�����������Ͳ����л�//���˲�ѯ��ť��������ť��ֹ
//�������ťʱ�����ö�ʱ������ʱ��ѯ��Ϣ������connect���������������
//���������̳߳ص���������ź�done_task(QObject *)����process_finished(QObject *)����
//do_task(QObject *, const QByteArray &)�ź������Ժ��̳߳ش���append_new(QObject *, const QByteArray &)
//��ʱ���࣬ʱ�䵽�ˣ��͵���sendMsgtoServer()�������ź�emitGetData��iter.value()->getHostAddr(),iter.value()->getHostPort()��
void SecuritySystem::on_btnStartMoniter_clicked()//����������ť
{
	if (isStart_==false)
	{
		//�Ż� �ڴ�й©
		pdataProcess_ = new DataProcess(this);

		pTaskThreadPool_ = new TaskThreadPool(this,pdataProcess_,Config::defenceCount);

		connect(pTaskThreadPool_,SIGNAL(done_task(QObject *)),this,SLOT(process_finished(QObject *)));

		connect(this,SIGNAL(do_task(QObject *, const QByteArray &)),pTaskThreadPool_,SLOT(append_new(QObject *, const QByteArray &)));

		isStart_ = true;
		ui_.systemAction->setDisabled(true);//������غ�������������ťΪ������
		ui_.hostManageAction->setDisabled(true);
		ui_.defenceAction->setDisabled(true);
		ui_.btnStartMoniter->setText(tr("ֹͣ���"));
		Utility::isMainWidget = true;

		//����ؼ�������һ���û�ѯ�ʣ���¼���ѯ�ʵ�ʱ��
		//��ֵtcpclients���map(clienquery,qdatatime)
		if (tcpClients_.size() < clientID_.size())//tcpclients_��clientquery��Ӧ��ʱ�䣬clientID�Ƕ�Ӧһ������
		{
			foreach (int i,clientID_)//������һ����ϣ��clientID��Qset��i��ֵ
			{
				//ԭ����Ҫ��i����ȥ��clientquery������������Ǵ���
				ClientQuery * client = new ClientQuery(this,i,mapDefenceBtn_[i],mapDefenceBtn_[i]->getHostAddr(), mapDefenceBtn_[i]->getHostPort());
				tcpClients_[client] = QDateTime::currentDateTime();
				mapDefenceBtn_[i]->SetDefenceStatus(DefenceStatus_normal);
			}
		}
		
		//�Լ�������һ��ʱ���࣬ʱ�䵽�ˣ��͵���sendMsgtoServer()
		if (highPriTimer_ == nullptr)
		{
			highPriTimer_ = new mmtimer(Config::timeGap,this);
			connect(highPriTimer_,SIGNAL(timeout()),this,SLOT(sendMsgtoServer()));
		}
		highPriTimer_->start();
	}
	else//Ŷ����������һ��bool�����ж�������û���������������̳߳أ��������ݴ����̣߳�����Ͽ��ź����ӣ���ʱֹͣ��
	{
		/*killTimer(Timer_);*/
		QList<ClientQuery*> listObj = tcpClients_.keys();//��ֵ��key-value
		foreach(ClientQuery * sock,listObj)
		{
			mapDefenceBtn_[sock->getDefenceID().toInt()]->SetDefenceStatus(DefenceStatus_init);
			sock->close();
		}
		//�Ż� �ڴ�й©
		tcpClients_.clear();
		//�Ͽ����ӣ��̳߳����������źš���������������ź�
		disconnect(pTaskThreadPool_,SIGNAL(done_task(QObject *)),this,SLOT(process_finished(QObject *)));
		disconnect(this,SIGNAL(do_task(QObject *, const QByteArray &)),pTaskThreadPool_,SLOT(append_new(QObject *, const QByteArray &)));
		//���ж�ʱ��ֹͣ
		if (highPriTimer_)
		{
			highPriTimer_->stop();
		}
		//������ʦ�ּӵģ���ֹͣ���ʱ��ֹͣ��ʱ����
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

		//�Ż� �ڴ�й©
		//bug, smart pointer
		//pdataProcess_->deleteLater();
		//pTaskThreadPool_->deleteLater();

		isStart_ = false;
		ui_.systemAction->setEnabled(true);
		ui_.hostManageAction->setEnabled(true);
		ui_.defenceAction->setEnabled(true);
		ui_.btnStartMoniter->setText(tr("�������"));

		Utility::isMainWidget = false;
	}
}

//�ĸ���ť�Ĳۺ�����ʵ�ֶ��ܼ򵥣�������Ӧ���ڶ������У������ж����ͷŵ����⣬ÿ�ε����Ҫ�½�����
void SecuritySystem::on_systemAction_triggered()//ϵͳ����
{
	//�Ż� �ڴ�й©
	SystemSetting *sys = new SystemSetting;
//	sys->setAttribute(Qt::WA_DeleteOnClose);
	sys->exec();
}
void SecuritySystem::on_hostManageAction_triggered()//��������
{
	//�Ż� �ڴ�й©
	HostSetting *host = new HostSetting;
//	host->setAttribute(Qt::WA_DeleteOnClose);
	host->exec();
}
void SecuritySystem::on_defenceAction_triggered()//��������
{
	//�Ż� �ڴ�й©
	DefenceSetting *defence=new DefenceSetting;
//	defence->setAttribute(Qt::WA_DeleteOnClose);
	defence->exec();
}
void SecuritySystem::on_queryAction_triggered()//�����ѯ
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

//�̳߳ط�����������ź�done_task(QObject *)�������������30�����ݿ�ʼ���ɣ���ͼ�о���Σ����ϢҲ����
void SecuritySystem::process_finished(QObject *obj)
{
	static int messageCount = 0;
	//�̰߳�ȫ����
 	ClientQuery *cli = static_cast<ClientQuery*>(obj);//����ת����һ������
	if (cli->isEventHappen())//���棬Σ���¼�����
	{
		if (messageCount >= 30 )//����30����������Ҫ�����µ�30��
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
		itemTime_[messageCount]->setText(cli->getEventTime().right(8));//��¼���¼���ʱ��
		itemArea_[messageCount]->setText(QString(cli->getDefenceID()));//��¼���¼�����������

		if (cli->isEventDanger())//�ڷ����¼��Ļ����ϣ����жϾ���ʲô�¼���Σ�ջ򾯸棬������Դ����Ż�
		{
			itemMessage_[messageCount]->setText(tr("Σ��"));
			switch (cli->getDefenceID().toInt())
			{ 
			case 1 :
				//����ʵ�ַ��;�����Ϣ���ֻ�
				if(isdanger1==false)
				{	
					isdanger1=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����1���������¼�������Σ��")).toAscii());
							}
						}
					}
					mytime1->start();	
				}
				break; 
			case 2 :
				if(isdanger2==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					isdanger2=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����2���������¼�������Σ��")).toAscii());
							}
						}
					}
					mytime2->start();
				}break; 
			case 3 :
				if(isdanger3==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					isdanger3=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����3���������¼�������Σ��")).toAscii());
							}
						}
					}
					mytime3->start();
				} break;
			case 4 :
				if(isdanger4==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					isdanger4=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����4���������¼�������Σ��")).toAscii());
							}
						}
					}
					mytime4->start();
				} break; 
			case 5 : 
				if(isdanger5==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					isdanger5=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����5���������¼�������Σ��")).toAscii());
							}
						}
					}
					mytime5->start();
				} break;
			case 6 : 
				if(isdanger6==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					isdanger6=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����6���������¼�������Σ��")).toAscii());
							}
						}
					}
					mytime6->start();
				}  break; 
			case 7 :  
				if(isdanger7==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					isdanger7=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����7���������¼�������Σ��")).toAscii());
							}
						}
					}
					mytime7->start();
				}  break; 
			case 8 :
				if(isdanger8==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					isdanger8=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(Config::contactTel))).append("\"")).append("\r")).toAscii());
								//Com_->write(QString("AT+CMGS=\"").append(stringToUnicode(tr("18716035785"))).append("\"").append("\r").toAscii());
								Com_->write(stringToUnicode(tr("����8���������¼�������Σ��")).toAscii());
							}
						}
					}
					mytime8->start();
				}  break;
			}
		}
		else
		{
			itemMessage_[messageCount]->setText(tr("����"));
			/*
			switch (cli->getDefenceID().toInt())
			{ 
			case 1 :
				//����ʵ�ַ��;�����Ϣ���ֻ�
				//
				if(iswarn1==false)
				{	
					iswarn1=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����1���������¼������𣺾���")).toAscii());
							}
						}
					}
					
					mytimew1->start();

				}break; 

			case 2 :
				if(iswarn2==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					iswarn2=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����2���������¼������𣺾���")).toAscii());
							}
						}
					}
					mytimew2->start();

				}break; 
			case 3 :
				if(iswarn3==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					iswarn3=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����3���������¼������𣺾���")).toAscii());
							}
						}
					}
					mytimew3->start();
				} break; 
			case 4 :
				if(iswarn4==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					iswarn4=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����4���������¼������𣺾���")).toAscii());
							}
						}
					}
					mytimew4->start();
				}  break; 
			case 5 : 
				if(iswarn5==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					iswarn5=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����5���������¼������𣺾���")).toAscii());
							}
						}
					}
					mytimew5->start();
				} break;
			case 6 : 
				if(iswarn6==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					iswarn6=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����6���������¼������𣺾���")).toAscii());
							}
						}
					}
					mytimew6->start();

				}  break; 
			case 7 :  
				if(iswarn7==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					iswarn7=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����7���������¼������𣺾���")).toAscii());
							}
						}
					}
					mytimew7->start();
				}  break; 
			case 8 :
				if(iswarn8==false)
				{	
					//����ʵ�ַ��;�����Ϣ���ֻ�
					iswarn8=true;
					if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
					{
						isSend = true;//����������ر����
						if (Com_)
						{
							if (Com_->isOpen())//������ڴ���
							{	
								Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
								Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
								Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
								Com_->write(stringToUnicode(tr("����8���������¼������𣺾���")).toAscii());
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

		QLabel *noticeLabel = mapDefenceLabel_[defenceID];//��IDӳ�䵽��Ӧ����ʾ��ǩ
		noticeLabel->setText(tr("__%1�����棬%2��Σ��_").arg(warnCount).arg(dangerCount));//��ͼ����ʾ�ı�ǩ
		int len = noticeLabel->text().size();//
		noticeLabel->setGeometry(btn->GetDefenceX()+Config::labelOffsetX,btn->GetDefenceY()+Config	::labelOffsetY, Config::labelWidth,Config::labelHeight);//�ñ�ǩ���õ���ʾ
		pixmap_ = pixmap_.scaled(noticeLabel->size());//�Ŵ�
		palette_.setBrush(QPalette::Window, QBrush(pixmap_));	//��ɫ��
		noticeLabel->setMask(pixmap_.mask());//ʵ�ֲ�������
		noticeLabel->setPalette(palette_);//
		noticeLabel->setVisible(true);//ʵ�ֿɼ�		
	}
}


void SecuritySystem::resetNotice(int ID)
{
	//�Ż� �ڴ�й©
	QLabel *noticeLabel = mapDefenceLabel_[ID];
	noticeLabel->setVisible(false);//��������ʹ������Ϣ��ǩ���ɼ������ɼ�֮��
	                               //�����ź��������������µ�����Ĵ����������е���ʾ��ǩ����new�ģ�Ӧ�ò������ڴ����⣬
	                               //Ӧ�ûἰʱ�ͷţ�
}

//�����ź�emitGetData
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

 //�յ����ݣ������ź�do_task��Ȼ���̳߳���pTaskThreadPool_����SLOT(append_new(QObject *, const QByteArray &))
 void SecuritySystem::new_data_recieved(QVector<QByteArray> data)
 {
	 QList<ClientQuery*> listObj = tcpClients_.keys();//tcpclients��key��clientsquery
	 foreach(ClientQuery * sock,listObj)
	 {
		 emit do_task(sock,data[sock->getDefenceID().toInt()-1]);//sock��clientsquery��data����һ����ά����
	 }                                                           //�����Ǹ�ѭ��������8���ź�
 }

 //������Ϣ��ʾ
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

void SecuritySystem::on_cbRecordAll_stateChanged(int state)//��¼ȫ�������Ǹ���ѡ��
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

void SecuritySystem::readComMessage() //�����ں���
{
	QByteArray temp = Com_->readAll();//��ȡ���ڻ��������������ݸ���ʱ����temp
	if(temp.indexOf('>')>=0)
	{
		char endCmd[1]={0X1A};
		Com_->write(endCmd);
		qDebug()<<" 444444444444"<<QDateTime ::currentDateTime().toString("hh:mm:ss.zzz");
		isSend=false;
		Com_->write(QString("AT+CNMI=1,1").append('\r').toAscii());
	}

	//�յ���Ϣλ��
	if(temp.indexOf("+CMTI:")>=0)
	{
		QString bytes(temp);
		QStringList strs = bytes.split(',');
		Com_->write(QString("AT+CMGR=%1").arg(strs[1]).append('\r').toAscii());		
	}
	//��ȡ���������Ϣ
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
			strout+="00"; //Ӣ�Ļ�����ǰ��"00"
		str0=str0.setNum(num,16);//���ʮ��������
		strout+=str0;
	}
	return strout;
}



void SecuritySystem::AutoSend1()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����1���������¼�������Σ��")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend2()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����2���������¼�������Σ��")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend3()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����3���������¼�������Σ��")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend4()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����4���������¼�������Σ��")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend5()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����5���������¼�������Σ��")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend6()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����6���������¼�������Σ��")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend7()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����7���������¼�������Σ��")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSend8()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����8���������¼�������Σ��")).toAscii());
			}
		}
	}
}


/*
void SecuritySystem::AutoSendw1()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	
				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����1���������¼������𣺾���")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw2()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����2���������¼������𣺾���")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw3()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����3���������¼������𣺾���")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw4()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����4���������¼������𣺾���")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw5()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����5���������¼������𣺾���")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw6()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����6���������¼������𣺾���")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw7()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����7���������¼������𣺾���")).toAscii());
			}
		}
	}
}
void SecuritySystem::AutoSendw8()
{
	//����ʵ�ַ��;�����Ϣ���ֻ�
	if (isSend == false)//�ж��Ƿ����ڷ��ͣ�û�����ڷ��������
	{
		isSend = true;//����������ر����
		if (Com_)
		{
			if (Com_->isOpen())//������ڴ���
			{	


				Com_->write(((QString("AT+CMGF=1")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSMP=17,167,2,25")).append('\r')).toAscii());
				Com_->write(((QString("AT+CSCS=\"UCS2\"")).append('\r')).toAscii());
				Com_->write(((((QString("AT+CMGS=\"")).append(stringToUnicode(tr("18716035785")))).append("\"")).append("\r")).toAscii());
				Com_->write(stringToUnicode(tr("����8���������¼������𣺾���")).toAscii());
			}
		}
	}
}

*/


		