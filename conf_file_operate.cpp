#include "conf_file_operate.h"
#include <QDebug>
#include <QRegExp>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>


ConfFileOperation::ConfFileOperation(QObject *parent)
    : QObject{parent}
{
}

bool ConfFileOperation::readIniFile(QString iniPath, IniInfo &iniInfo)
{
    QFile file(iniPath);
    if(!file.open(QIODevice::ReadOnly)){
        emit sigConfFileOpLog("Read ini json error.");
        return false;
    }
    QByteArray data(file.readAll());
    file.close();

    QJsonParseError jError;	//创建QJsonParseError对象
    //使用QJsonDocument的fromJson函数读取json串，并将QJsonParseError对象传入获取错误值
    QJsonDocument jDoc = QJsonDocument::fromJson(data, &jError);
    //判断QJsonParseError对象获取的error是否包含错误，包含则返回0
    if(jError.error != QJsonParseError::NoError){
        emit sigConfFileOpLog("Json format error");
        return false;
    }
    QJsonObject rootObj = jDoc.object();
    QJsonArray RecentFile = rootObj["RecentFile"].toArray();
    for(int i = 0; i < RecentFile.size(); ++i){
        QString value = RecentFile.at(i).toString();
        iniInfo.m_RecentFile.push_back(value);
    }
    //0.HostName
    iniInfo.m_HostName = rootObj["HostName"].toString();
    iniInfo.m_Version = rootObj["Version"].toString();
    iniInfo.m_Date = rootObj["Date"].toString();
    iniInfo.m_IniPath = iniPath;
    return true;
}

bool ConfFileOperation::readUserFile(QString usrPath, UserInfo &userInfo)
{
    QFile file(usrPath);
    if(!file.open(QIODevice::ReadOnly)){
        emit sigConfFileOpLog("Read usr json error");
        return false;
    }
    QByteArray data(file.readAll());
    file.close();

    QJsonParseError jError;	//创建QJsonParseError对象
    //使用QJsonDocument的fromJson函数读取json串，并将QJsonParseError对象传入获取错误值
    QJsonDocument jDoc = QJsonDocument::fromJson(data, &jError);
    //判断QJsonParseError对象获取的error是否包含错误，包含则返回0
    if(jError.error != QJsonParseError::NoError){
        emit sigConfFileOpLog("Json format error");
        return false;
    }
    QJsonObject jObj = jDoc.object();
    userInfo.m_UserPath = usrPath;
    analysisJson(jObj, userInfo);
    return true;
}

void ConfFileOperation::analysisJson(QJsonObject &rootObj,UserInfo &userInfo){
    //1. 读取RepoPath
    QJsonArray RepoPathArr = rootObj["RepoPath"].toArray();
    //2. 获取RepoPathObj中指定键的值（值对应的类型直接使用对应函数转）
    for(int i = 0; i < RepoPathArr.size(); ++i){
        QJsonObject  data = RepoPathArr.at(i).toObject();
        RepoNamePath namePath;
        namePath.m_Name = data.keys().at(0);  // 只有一个
        namePath.m_Path = data[namePath.m_Name].toString();
        userInfo.m_RepoPathList.push_back(namePath);
    }

    QJsonObject SoftPathMap = rootObj["Software"].toObject();
    for(int i = 0; i < SoftPathMap.keys().size(); ++i){
        QString key = SoftPathMap.keys().at(i);
        QString value = SoftPathMap[key].toString();
        if(key == "Typora") {
            userInfo.m_TyporaPath = value;
        }else {
            userInfo.m_VsCodePath = value;
        }
    }

    QJsonObject DictPathMap = rootObj["Dictionary"].toObject();
    for(int i = 0; i < DictPathMap.keys().size(); ++i){
        QString key = DictPathMap.keys().at(i);
        QString value = DictPathMap[key].toString();
        if(key == "TranslateWeb") {
            userInfo.m_TranslateWeb = value;
        }else if(key == "CambridgeWordWeb"){
            userInfo.m_CambridgeWordWeb = value;
        }else if(key == "CambridgeSentWeb"){
            userInfo.m_CambridgeSentWeb = value;
        }else{ //
            userInfo.m_PronunciationWeb = value;
        }
    }
    userInfo.m_Symbols = rootObj["Symbols"].toString();
}
