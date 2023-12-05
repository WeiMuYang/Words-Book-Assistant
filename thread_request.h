#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include <QObject>
#include "network_access.h"
#include "data_type.h"

//// worker

#include <QThread>
/**
 * @brief Worker类一定要继承自QObject，才能移到QThread
 */
class Worker : public QObject
{
    Q_OBJECT
public:
    // 1. 继承QObject
    explicit Worker(QObject *parent = nullptr);
    ~Worker();
public slots:
    // 2. 线程要执行的槽函数，执行完成后可以发结束信号
    void doWorkAccessWord(const QString& TranslateWeb, QString word);
    void addWordSlot(WordSentInfo wordInfo);
    void doWorkAccessSent(const QString& TranslateWeb, QString sent);
    void addSentSlot(WordSentInfo wordInfo);
    void getLogSlot(QString msg);

signals:
    // 3. 发送包含执行的结果的信号
    void resultWordSend(WordSentInfo result);
    void resultSentSend(WordSentInfo result);
    // 4. 发送执行结束的信号
    void endThrSend();
    void sigWorkerLog(QString msg);
private:
    NetworkAccess* netWorkAccess_;
};


//// Controller

// 用于进行线程控制
class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

public slots:
    // 1. 接收执行完毕信号的槽函数
    void workEndDisplay();
    // 2. 接收执行结果的槽函数
    void handleWordResults(WordSentInfo result);
    void handleSentResults(WordSentInfo result);
    void getLogSlot(QString msg);
signals:
    // 3. 发送启动doWork的信号
    void operateWord(const QString& TranslateWeb, QString word);
    void operateSent(const QString& TranslateWeb, QString word);
    void sendWord(WordSentInfo ws);
    void sendSent(WordSentInfo ws);
    void sigControllerLog(QString msg);
private:
    // 4. 子线程指针，指向运行doWork函数的线程对象
    QThread* m_pthr_doWork;
    // 5. worker对象，包含doWork函数的实体
    Worker* m_worker_obj;
};



#endif // NETWORKTHREAD_H
