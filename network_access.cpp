#include "network_access.h"
#include <QNetworkReply>
#include <QDebug>
#include <QSslSocket>
#include <QRegExp>
#include <QTextCodec>

const QString wordSplit = "<span class=\"pos\"";
const QString phonetic = ">英</span><span class=\"phonetic\"";

NetworkAccess::NetworkAccess(QObject *parent)
    : QObject{parent}
{
    manager_ = new QNetworkAccessManager(this);
    translateWeb_ = "https://dict.youdao.com/result?word=%word%&lang=en";
    qDebug() << "OpenSSL支持情况:" << QSslSocket::supportsSsl();
}

void NetworkAccess::accessWord(QString word){
    wordSentence_ = word;
    QString web = translateWeb_;
    web = web.replace("%word%" ,word);
    request_.setUrl(QUrl(web));
    reply_ = manager_->get(request_);
    QObject::connect(reply_, &QNetworkReply::finished, this, &NetworkAccess::replyWordFinishedSlot);
}

void NetworkAccess::replyWordFinishedSlot(){
    if(reply_->error() == QNetworkReply::NoError){
        // 读取响应数据
        QByteArray data = reply_->readAll();
        // 处理响应数据，例如保存为音频文件或进行播放
        analysisWordInfo(data);
        emit sigNetworkAccessLog("接收单词数据成功!");
    }else{
        // 处理请求错误
        emit sigNetworkAccessLog("接收单词数据失败: " + reply_->errorString());
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
    emit sigNetworkAccessLog("正在解析单词！");
    QString dataStr = QString(Data);
    QStringList listStr = dataStr.split(wordSplit);
    // 当前单词信息
    WordSentInfo curWordInfo;
    curWordInfo.m_isWord = true;
    if(listStr.size() > 1){
        curWordInfo.m_WordSent = wordSentence_;
        curWordInfo.m_Phonetic_UK = getWordPhonetic(listStr.first());
        for(int i = 1; i < listStr.size()-1; ++i) {
            curWordInfo.m_Translation.append(delWordExCharacters(listStr.at(i)));
        }
        int pos = listStr.last().indexOf("</li>");
        curWordInfo.m_Translation.append(delWordExCharacters(listStr.last().remove(pos+5, listStr.last().size())));
        emit sendWordInfo(curWordInfo);
    }
    else{
        emit sigNetworkAccessLog("翻译单词出错，未查到 \"" + wordSentence_ + "\"！");
    }
}

// Sentence
void NetworkAccess::analysisSentenceInfo(const QByteArray& Data) {
    emit sigNetworkAccessLog("正在解析句子！");
    QString dataStr = QString(Data);
    int pos = dataStr.indexOf("trans-content");
    dataStr = dataStr.remove(0, pos);
    pos = dataStr.indexOf('<');
    dataStr = dataStr.remove(pos, dataStr.size()-1);
    pos = dataStr.indexOf('>');
    dataStr = dataStr.remove(0, pos+1);
    // 当前单词信息
    if(!dataStr.isEmpty()){
        WordSentInfo curWordInfo;
        curWordInfo.m_isWord = false;
        curWordInfo.m_WordSent = wordSentence_;
        curWordInfo.m_Translation.append(dataStr);
        emit sendSentenceInfo(curWordInfo);
    }
    else{
        emit sigNetworkAccessLog("翻译句子出错，未查到 \"" + wordSentence_ + "\"！");
    }
}

void NetworkAccess::replySentenceFinishedSlot()
{
    if(reply_->error() == QNetworkReply::NoError){
        // 读取响应数据
        QByteArray data = reply_->readAll();
        // 处理响应数据，例如保存为音频文件或进行播放
        analysisSentenceInfo(data);
        emit sigNetworkAccessLog("接收单词数据成功!");
    }else{
        // 处理请求错误
        emit sigNetworkAccessLog("接收单词数据失败: " + reply_->errorString());
    }
    // 释放资源
    reply_->deleteLater();
}

void NetworkAccess::accessSentence(QString sentence){
    wordSentence_ = sentence;
    QString encodedString = QUrl::toPercentEncoding(sentence);
    QString web = translateWeb_;
    web = web.replace("%word%" ,encodedString);
    request_.setUrl(QUrl(web));
    // 发送请求并获取响应
    reply_ = manager_->get(request_);
    // 当请求完成时，响应数据可用
    QObject::connect(reply_, &QNetworkReply::finished, this, &NetworkAccess::replySentenceFinishedSlot);
}
