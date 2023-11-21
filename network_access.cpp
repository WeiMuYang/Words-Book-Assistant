#include "network_access.h"
#include <QNetworkReply>
#include <QDebug>
#include <QSslSocket>
#include <QRegExp>

const QString wordSplit = "<span class=\"pos\"";
const QString phonetic = ">英</span><span class=\"phonetic\"";
//const QString wordSubStringEnd = " class=\"pos\">";

NetworkAccess::NetworkAccess(QObject *parent)
    : QObject{parent}
{
    manager_ = new QNetworkAccessManager(this);
    qDebug() << "OpenSSL支持情况:" << QSslSocket::supportsSsl();
}

void NetworkAccess::accessWord(QString word){
    wordSentence_ = word;
    request_.setUrl(QUrl(u8"https://dict.youdao.com/result?word=" + word + "&lang=en"));
    // 发送请求并获取响应
    reply_ = manager_->get(request_);
    // 当请求完成时，响应数据可用
    QObject::connect(reply_, &QNetworkReply::finished, this, &NetworkAccess::replyWordFinishedSlot);
}

void NetworkAccess::replyWordFinishedSlot(){
    if(reply_->error() == QNetworkReply::NoError){
        // 读取响应数据
        QByteArray data = reply_->readAll();
        // 处理响应数据，例如保存为音频文件或进行播放
        analysisWordInfo(data);
        qDebug() << "Speech data received successfully.";
    }else{
        // 处理请求错误
        qDebug() << "Error occurred: " << reply_->errorString();
    }

    // 释放资源
    reply_->deleteLater();
}

QString NetworkAccess::delWordExCharacters(QString text){
    // 1. 删除<>内字符
    QRegExp tagRegex("<[^>]*>");
    QString result = text.replace(tagRegex, " ");
    // 2. 删除>前面的字符
    int pos = result.indexOf('>');
    if (pos != -1) {
        result.remove(0, pos + 1); // 删除'> '及前面的字符
    }
    return result;
}

QString NetworkAccess::getWordPhonetic(QString phoneticStr) {
    int pos = phoneticStr.indexOf(phonetic);
    phoneticStr = phoneticStr.remove(0, pos + 1);
    QStringList list = phoneticStr.split("/");
    if(list.size() > 3) {
        return list.at(2);
    }
    return QString("");
}

void NetworkAccess::analysisWordInfo(const QByteArray& Data) {
    QString dataStr = QString(Data);
    QStringList listStr = dataStr.split(wordSplit);
    // 当前单词信息
    WordInfo curWordInfo;
    if(listStr.size() > 1){
        curWordInfo.m_word = wordSentence_;
        curWordInfo.m_Phonetic_UK = getWordPhonetic(listStr.first());
        for(int i = 1; i < listStr.size()-1; ++i) {
            qDebug() << listStr.at(i);
            curWordInfo.m_Translation.append(delWordExCharacters(listStr.at(i)));
        }
        int pos = listStr.last().indexOf("</li>");
        curWordInfo.m_Translation.append(delWordExCharacters(listStr.last().remove(pos+5, listStr.last().size())));
        qDebug() << curWordInfo.m_Phonetic_UK;
        qDebug() << curWordInfo.m_Translation;
        emit sendWordInfo(curWordInfo);
    }
    else{
        qDebug() << "分割HTML字符串出错！";
    }
}

// Sentence

void NetworkAccess::analysisSentenceInfo(const QByteArray& Data) {
    QString dataStr = QString(Data);
    int pos = dataStr.indexOf("trans-content");
    dataStr = dataStr.remove(0, pos);
    pos = dataStr.indexOf('<');
    dataStr = dataStr.remove(pos, dataStr.size()-1);
    pos = dataStr.indexOf('>');
    dataStr = dataStr.remove(0, pos+1);
    qDebug() << dataStr;
    // 当前单词信息
    if(!dataStr.isEmpty()){
        WordInfo curWordInfo;
        curWordInfo.m_word = wordSentence_;
        curWordInfo.m_Translation.append(dataStr);
        emit sendSentenceInfo(curWordInfo);
    }
    else{
        qDebug() << "分割HTML字符串出错！";
    }
}

void NetworkAccess::replySentenceFinishedSlot()
{
    if(reply_->error() == QNetworkReply::NoError){
        // 读取响应数据
        QByteArray data = reply_->readAll();
        // 处理响应数据，例如保存为音频文件或进行播放
        analysisSentenceInfo(data);
        qDebug() << "Speech data received successfully.";
    }else{
        // 处理请求错误
        qDebug() << "Error occurred: " << reply_->errorString();
    }

    // 释放资源
    reply_->deleteLater();
}

void NetworkAccess::accessSentence(QString sentence){
    wordSentence_ = sentence;
    QByteArray encodedString = QUrl::toPercentEncoding(sentence);
    request_.setUrl(QUrl(u8"https://dict.youdao.com/result?word=" + encodedString + "&lang=en"));
    // 发送请求并获取响应
    reply_ = manager_->get(request_);
    // 当请求完成时，响应数据可用
    QObject::connect(reply_, &QNetworkReply::finished, this, &NetworkAccess::replySentenceFinishedSlot);
}
