#include "network_access.h"
#include <QNetworkReply>
#include <QDebug>
#include <QSslSocket>


NetworkAccess::NetworkAccess(QObject *parent)
    : QObject{parent}
{
    manager_ = new QNetworkAccessManager(this);
    qDebug()<< QSslSocket::sslLibraryBuildVersionString();
    Access();
}

void NetworkAccess::Access(){
    request_.setUrl(QUrl(u8"http://dict.youdao.com/result?word=like&lang=en"));
    // 发送请求并获取响应
//    QNetworkAccessManager* manager_ = new QNetworkAccessManager(this);
    reply_  = manager_->get(request_);
    // 当请求完成时，响应数据可用
    QObject::connect(reply_, &QNetworkReply::finished, this, &NetworkAccess::replyFinished);
}

void NetworkAccess::replyFinished(){
    if(reply_->error() == QNetworkReply::NoError){
        // 读取响应数据
        QByteArray data = reply_->readAll();

        // 处理响应数据，例如保存为音频文件或进行播放
        // ...

        qDebug() << "Speech data received successfully.";
    }else{
        // 处理请求错误
        qDebug() << "Error occurred: " << reply_->errorString();
    }

    // 释放资源
    reply_->deleteLater();
}
