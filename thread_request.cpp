#include "thread_request.h"
#include <QDebug>

/**
 * @brief 构造函数
 * @param parent:
 */
Worker::Worker(QObject *parent) : QObject(parent)
{
    qDebug() << "Worker()" << "thread:" << QThread::currentThreadId();
    netWorkAccess_ = new NetworkAccess(this);

    qRegisterMetaType<WordSentInfo>("WordSentInfo");
}


Worker::~Worker()
{
    qDebug() << "~Worker()" << "thread:" << QThread::currentThreadId();
}



/**
 * @brief 线程要执行的函数
 * @param parameter：     从信号中传递的参数，只执行自增
 */
void Worker::doWorkAccessWord(const QString& TranslateWeb, QString word)                        //doWork定义了线程要执行的操作
{
    qDebug()<<"do work thread ID:"<< QThread::currentThreadId();

    // network
    netWorkAccess_->setTranslateWeb(TranslateWeb);
    netWorkAccess_->accessWord(word);
    connect(netWorkAccess_,&NetworkAccess::sendWordInfo,this, &Worker::addWordSlot);
}

void Worker::addWordSlot(WordSentInfo wordInfo){
    emit resultWordSend(wordInfo);    // 发送工作的结果
    emit endThrSend();             // 发送结束信号
}

void Worker::doWorkAccessSent(const QString& TranslateWeb, QString sent)                        //doWork定义了线程要执行的操作
{
    // network
    netWorkAccess_->setTranslateWeb(TranslateWeb);
    netWorkAccess_->accessWord(sent);
    connect(netWorkAccess_,&NetworkAccess::sendSentenceInfo,this, &Worker::addSentSlot);
}


void Worker::addSentSlot(WordSentInfo wordInfo){
    emit resultSentSend(wordInfo);    // 发送工作的结果
    emit endThrSend();             // 发送结束信号
}

///////////////////////////////// control ///////////////////////////////

Controller::Controller(QObject *parent) : QObject(parent) {
    // 1. 创建对象
    m_pthr_doWork = new QThread;
    // m_pthr_doWork->setParent(this);   // 线程最好不用指定父类对象，否则可能内出错
    m_worker_obj = new Worker;
    // 2. 将包含dowork的函数对象，移动到子线程
    m_worker_obj->moveToThread(m_pthr_doWork);

    // 3. 线程结束后，退出线程并销毁线程
    connect(m_pthr_doWork, SIGNAL(finished()), m_pthr_doWork, SLOT(deleteLater()));
    // 4. 收到操作信号，线程运行worker的doWork槽函数
    connect(this, &Controller::operateWord, m_worker_obj, &Worker::doWorkAccessWord);
    connect(this, &Controller::operateSent, m_worker_obj, &Worker::doWorkAccessWord);
    // 5. 收到worker的结果信号后，将结果发送给handleResults槽函数
    connect(m_worker_obj, &Worker::resultWordSend, this, &Controller::handleWordResults);
    connect(m_worker_obj, &Worker::resultSentSend, this, &Controller::handleSentResults);
    // 6. 收到worker的结束信号后，执行任务结束显示槽函数workEndDisplay
    connect(m_worker_obj, &Worker::endThrSend, this, &Controller::workEndDisplay);
    // 7. 启动线程
    m_pthr_doWork->start();
}

Controller::~Controller()
{

    //   delete m_pthr_doWork;   // 不起作用
}


/**
 * @brief 线程结束后，退出线程并销毁线程
 */
void Controller::workEndDisplay()
{
    qDebug() << "workEndDisplay: Thread run finished and work end;";
    // * 注意：加上下面的两条语句，线程只能执行一次，然后就结束
    m_pthr_doWork->quit();
    m_pthr_doWork->wait();
    delete m_pthr_doWork;
}

/**
 * @brief 获取线程中函数执行的结果，并显示当前的线程ID
 * @param rslt： 接收发送的结果信号
 */
void Controller::handleWordResults(WordSentInfo result)   //处理线程执行的结果
{
    qDebug()<<"Controller::handleResults thread ID:"<< QThread::currentThreadId()<<'\n';
    qDebug()<<"the last result is:"<<result.m_WordSent;
    sendWord(result);
}

void Controller::handleSentResults(WordSentInfo result)   //处理线程执行的结果
{
    qDebug()<<"Controller::handleResults thread ID:"<< QThread::currentThreadId()<<'\n';
    qDebug()<<"the last result is:"<<result.m_WordSent;
    sendSent(result);
}
