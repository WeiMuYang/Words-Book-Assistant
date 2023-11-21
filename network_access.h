#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QObject>
#include <QNetworkAccessManager>
#include "data_type.h"

class NetworkAccess : public QObject
{
    Q_OBJECT
public:
    explicit NetworkAccess(QObject *parent = nullptr);

    QString delWordExCharacters(QString text);
    QString getWordPhonetic(QString phoneticStr);

public slots:
    void accessWord(QString word);
    void accessSentence(QString sentence);
    void replyWordFinishedSlot();
    void replySentenceFinishedSlot();
    void analysisWordInfo(const QByteArray& Data);
    void analysisSentenceInfo(const QByteArray& Data);
signals:
    void sendWordInfo(WordInfo info);
    void sendSentenceInfo(WordInfo info);
private:
    QNetworkAccessManager *manager_;
    // 创建一个 GET 请求
    QNetworkRequest request_;
    // 发送请求并获取响应
    QNetworkReply *reply_;

    QString wordSentence_;
};

#endif // NETWORKACCESS_H
