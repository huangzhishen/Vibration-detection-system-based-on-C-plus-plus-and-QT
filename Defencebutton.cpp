#include "Defencebutton.h"
#include "Config.h"
#include "CheckData.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QFont>
#include <QMutexLocker>
#include <QBitmap>


//为什么有两个构造函数呢？
DefenceButton::DefenceButton(QWidget *parent) 
	: QWidget(parent),mtx_(),isAlert(false)
{
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->defenceID="";
    this->defenceType=DefenceType_1;
    this->hostID="";
    this->hostName="";
    this->defenceSound="";
    this->defenceImage="";
    this->defenceUser="admin";
    this->defenceX=0;
    this->defenceY=0;
    this->defenceStatus=DefenceStatus_normal;
	initForm();
}


DefenceButton::DefenceButton(QWidget *parent, QString defenceID, DefenceType defenceType,QString hostID,QString hostName,
								 QString defenceSound,QString defenceImage, QString defenceUser,int defenceX, int defenceY,
								 DefenceStatus defenceStatus)
	:QWidget(parent),mtx_(),isAlert(false)
{
    this->setCursor(QCursor(Qt::PointingHandCursor));//设置鼠标移动到该组件时的形状（小手指）
    this->defenceID=defenceID;
    this->defenceType=defenceType;
    this->hostID=hostID;
    this->hostName=hostName;
    this->defenceSound=defenceSound;
    this->defenceImage=defenceImage;
    this->defenceUser=defenceUser;
    this->defenceX=defenceX;
    this->defenceY=defenceY;
    this->defenceStatus=defenceStatus;
	initForm();
}

void DefenceButton::initForm()
{
	//优化 内存泄漏
	currentRG = new QRadialGradient(0,0,80);//在坐标（0，0）的位置上建一个半径是80的渐变圆
	labelOfNum = new QRadialGradient(0,0,80);
	labelOfNum->setColorAt(0, QColor(50, 50, 50));//颜色设置
	labelOfNum->setColorAt(0.6, QColor(30, 30, 30));
	labelOfNum->setColorAt(1, QColor(10, 10, 10));

	isChange=false;
	//优化 内存泄漏
	myTimer=new QTimer(this);
	myTimer->setInterval(500);
	connect(myTimer,SIGNAL(timeout()),this,SLOT(alertChange()));
	this->installEventFilter(this);//安装监听器,监听鼠标拖动；即事件过滤器
	SetDefenceStatus(defenceStatus);

}

bool DefenceButton::eventFilter(QObject *, QEvent *evt)
{
	if (Utility::isMainWidget && evt->type() == QEvent::MouseButtonPress )
	{
		QMouseEvent* e = static_cast<QMouseEvent*>(evt);
		if (e->button() == Qt::LeftButton)
		{
			//if (defenceStatus == DefenceStatus_suspicous)
			//{
				CheckData *w;
				{
					QMutexLocker locker(&mtx_);
					w = new CheckData(warnningList_,dangerList_,"信号");
					connect(w,SIGNAL(resetSignal()),this,SLOT(resetList()));
				}
				w->setAttribute(Qt::WA_DeleteOnClose);
				w->show();
		/*	}
			else if (defenceStatus == DefenceStatus_alert)
			{
				CheckData *w;
				{
					QMutexLocker locker(&mtx_);				
					w= new CheckData(dangerList_,"危险信号");
					connect(w,SIGNAL(resetSignal()),this,SLOT(resetList()));
				}
				w->show();
			}*/
		}
	}//这个是防区按钮移动
    if (Config::isMove)
	{
        static QPoint lastPnt;
        static bool isHover = false;
        if(evt->type() == QEvent::MouseButtonPress)
		{
            QMouseEvent* e = static_cast<QMouseEvent*>(evt);
            if(this->rect().contains(e->pos()) && (e->button() == Qt::LeftButton))
			{
                lastPnt = e->pos();
                isHover = true;
            }
        }
		else if(evt->type() == QEvent::MouseMove && isHover)
		{
            QMouseEvent* e = static_cast<QMouseEvent*>(evt);
            int dx=e->pos().x() - lastPnt.x();
            int dy=e->pos().y()-lastPnt.y();
            this->move(this->x()+dx,this->y()+dy);
        }
		else if(evt->type() == QEvent::MouseButtonRelease && isHover)
		{
            isHover = false;
        }
    }
    return false;
}

// 定时器0.5s触发一次
// 666这个设计的挺好呀，用一个bool变量就实现了一个防区按钮根据警情的闪烁
// 这个bool变量与定时器的结合使用，简直完美呀
void DefenceButton::alertChange()
{
	if (isChange)
	{
		isChange = false;
		if (defenceStatus == DefenceStatus_normal)
		{
			currentRG->setColorAt(0, QColor(68, 187, 92));
			currentRG->setColorAt(0.6, QColor(59, 162, 80));
			currentRG->setColorAt(1, QColor(54, 150, 74));
		}
		else if (defenceStatus == DefenceStatus_suspicous)
		{
			currentRG->setColorAt(0, QColor(255, 255, 100));
			currentRG->setColorAt(0.6, QColor(255, 255, 70));
			currentRG->setColorAt(1, QColor(230, 230, 50));
		}
		else if (defenceStatus == DefenceStatus_alert)
		{
			currentRG->setColorAt(0, QColor(255, 0, 0));
			currentRG->setColorAt(0.6, QColor(230, 0, 0));
			currentRG->setColorAt(1, QColor(204, 0, 0));
		}
	}
	else
	{
		isChange = true;
		currentRG->setColorAt(0, QColor(50, 50, 50));
		currentRG->setColorAt(0.6, QColor(30, 30, 30));
		currentRG->setColorAt(1, QColor(10, 10, 10));
	}
    update();
}

void DefenceButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    //设置坐标和缩放
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing); 	/* 使用反锯齿(如果可用) */
    painter.translate(width() / 2, height() / 2);	/* 坐标变换为窗体中心 */
    int side = qMin(width(), height());
    painter.scale(side / 200.0, side / 200.0);		/* 比例缩放 */
    painter.setPen(Qt::NoPen);

    //画外边框
    int radius = 100;
    QLinearGradient lg1(0, -radius, 0, radius);
    lg1.setColorAt(0, QColor(255, 255, 255));
    lg1.setColorAt(1, QColor(166, 166, 166));
    painter.setBrush(lg1);
    painter.drawEllipse(-radius, -radius, radius << 1, radius << 1);

    //画内边框
    radius -= 13;
    QLinearGradient lg2(0, -radius, 0, radius);
    lg2.setColorAt(0, QColor(166, 166, 166));
    lg2.setColorAt(1, QColor(255, 255, 255));
    painter.setBrush(lg2);
    painter.drawEllipse(-radius, -radius, radius << 1, radius << 1);

    //画内部指示颜色
    radius -= 4;
    painter.setBrush(*currentRG);
    painter.drawEllipse(-radius, -radius, radius << 1, radius << 1);
    painter.restore();

    //画防区号文字
    painter.save();
    QFont font("Arial",10,QFont::Bold,false);
    painter.setFont(font);
    QPen pen(QColor(255,255,255));
    painter.setPen(pen);
	
    painter.drawText(rect(),Qt::AlignCenter,this->defenceID);
    painter.restore();
}

void DefenceButton::SetDefenceID(QString defenceID)
{
    if (this->defenceID!=defenceID)
	{
        this->defenceID=defenceID;
        update();
    }
}

//这个设定按钮状态的函数有点怪，准确来说本函数是设置按钮的颜色
void DefenceButton::SetDefenceStatus(DefenceStatus defenceStatus)
{
	if (!isAlert)
	{
		this->defenceStatus=defenceStatus;
	}
	if (defenceStatus == DefenceStatus_init)
	{
		myTimer->stop();
		currentRG->setColorAt(0, QColor(50, 50, 50));
		currentRG->setColorAt(0.6, QColor(30, 30, 30));
		currentRG->setColorAt(1, QColor(10, 10, 10));
		update();
	}
	else if (defenceStatus == DefenceStatus_select)
	{
		myTimer->stop();
		currentRG->setColorAt(0, QColor(145, 185, 235));
		currentRG->setColorAt(0.6, QColor(120, 170, 215));
		currentRG->setColorAt(1, QColor(102, 153, 204));
		update();
	}
	else 
	{
		if (defenceStatus == DefenceStatus_alert)
		{
			isAlert = true;
		}

		if (!myTimer->isActive())
		{
			myTimer->start();
		}
	}  
}

//其他线程调用
void DefenceButton::appendWarnList(QString defenceID,QString currTime)
{
	QMutexLocker locker(&mtx_);
	warnningList_[currTime] = defenceID;
}

void DefenceButton::appendDangerList(QString defenceID,QString currTime)
{
	QMutexLocker locker(&mtx_);
	dangerList_[currTime] = defenceID;
}

//主线程调用
void DefenceButton::resetList()
{
	QMutexLocker locker(&mtx_);
	warnningList_.clear();
	dangerList_.clear();
	isAlert = false;
	SetDefenceStatus(DefenceStatus_normal);
	emit sig_resetList(defenceID.toInt());
}

void DefenceButton::setHostAddrAndPort(QString hostAddr,int hostPort)
{
	hostAddr_ = hostAddr;
	hostPort_ = hostPort;
}

QString DefenceButton::getHostAddr()
{
	return hostAddr_;
}

int DefenceButton::getHostPort()
{
	return hostPort_;
}