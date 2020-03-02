# Vibration-detection-system-based-on-C-plus-plus-and-QT
Vibration detection system based on C plus plus and QT


基于QT的实时震动监测系统    
(1)、项目简介：该项目开发一桌面应用，对约5km长的马路进行震动监测，以防止挖掘机等异常事件破坏地面下的电缆。如有剧烈震动，则发送短信至负责人。经测试，危险发生到收到报警短信的时间相差1s，分类准确率达到98%，实时性与准确度较高。
(2)、开发时间：2018年3月--2018年10月
(3)、开发环境与编程语言：Windows、VS2010、QT、OpenCV、C++
(4)、项目技术点：
a)、信号采集：对8路信号的实时采集、分帧、小波去噪、提取mfcc特征、去除静默段、提取信号频率与能量特征。
b)、信号分类：利用二级阈值对信号分类、应用支持向量机算法对信号分类、GMM+UBM声纹识别算法进行分类。
c)、主线程负责采集数据，当得到完整的数据（10000点）后，添加进线程池完成数据处理，线程池减少创建线程的开销，提高数据处理的速度，减少延迟，以获得更高的实时性。
d)、利用FIFO队列思想在一个GSM模块实现报警短信多人定时发送。
e)、在多线程中利用全局函数通过加锁的方式实现信号数据的MySQL数据库存储，并通过UI组件实现原始信号与差分信号任意时间长度的波形图展示。
f)、利用shared_ptr智能指针管理资源，避免内存泄漏。
