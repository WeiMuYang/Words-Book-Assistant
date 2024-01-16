#ifndef CONFFILEOPERATION_H
#define CONFFILEOPERATION_H

#include <QObject>
#include "data_type.h"
class ConfFileOperation : public QObject
{
    Q_OBJECT
public:
    explicit ConfFileOperation(QObject *parent = nullptr);

    bool readIniFile(QString iniPath, IniInfo &iniInfo);
    bool readUserFile(QString usrPath,UserInfo &userInfo);
    void analysisJson(QJsonObject &rootObj,UserInfo &userInfo);

signals:
    void sigConfFileOpLog(QString s);

};

#endif // CONFFILEOPERATION_H
