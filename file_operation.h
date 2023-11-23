#ifndef OPERATEFILE_H
#define OPERATEFILE_H

#include <QObject>
#include "data_type.h"
class FileOperation : public QObject
{
    Q_OBJECT
public:
    explicit FileOperation(QObject *parent = nullptr);

    bool readIniFile(QString iniPath, IniInfo &iniInfo);
    bool readUserFile(QString usrPath,UserInfo &userInfo);
    void analysisJson(QJsonObject &rootObj,UserInfo &userInfo);

public slots:
    bool appendWord(WordSentInfo wordInfo);
    bool appendSentence(WordSentInfo sentence);

signals:

private:
    QString wordPath_;
    QString cambridgeWordWeb_;
    QString cambridgeSentWeb_;
    QString pronunciationWeb_;
};

#endif // OPERATEFILE_H
