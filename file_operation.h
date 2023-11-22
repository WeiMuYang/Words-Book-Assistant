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
    bool appendWord(WordInfo wordInfo);
    bool appendSentence(WordInfo sentence);

signals:

private:
    QString wordPath_;
    QString website_;
    QString audio_;
};

#endif // OPERATEFILE_H
