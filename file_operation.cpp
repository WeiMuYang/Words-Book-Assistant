#include "file_operation.h"
#include <QFile>
#include <QDebug>
#include <QRegExp>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "data_type.h"

FileOperation::FileOperation(QObject *parent)
    : QObject{parent}
{
    wordPath_ = "../words-doc.md";
    website_ = "https://dictionary.cambridge.org/dictionary/english-chinese-simplified/";
    audio_ = "http://dict.youdao.com/dictvoice?type=0&audio=";
}

bool FileOperation::readIniFile(QString iniPath, IniInfo &iniInfo)
{
    QFile file(iniPath);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "read ini json error";
        return false;
    }
    QByteArray data(file.readAll());
    file.close();

    QJsonParseError jError;	//创建QJsonParseError对象
    //使用QJsonDocument的fromJson函数读取json串，并将QJsonParseError对象传入获取错误值
    QJsonDocument jDoc = QJsonDocument::fromJson(data, &jError);
    //判断QJsonParseError对象获取的error是否包含错误，包含则返回0
    if(jError.error != QJsonParseError::NoError){
        qDebug()<<"json format error";
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

bool FileOperation::readUserFile(QString usrPath, UserInfo &userInfo)
{
    QFile file(usrPath);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "read usr json error";
        return false;
    }
    QByteArray data(file.readAll());
    file.close();

    QJsonParseError jError;	//创建QJsonParseError对象
    //使用QJsonDocument的fromJson函数读取json串，并将QJsonParseError对象传入获取错误值
    QJsonDocument jDoc = QJsonDocument::fromJson(data, &jError);
    //判断QJsonParseError对象获取的error是否包含错误，包含则返回0
    if(jError.error != QJsonParseError::NoError){
        qDebug() << "json format error";
        return false;
    }
    QJsonObject jObj = jDoc.object();
    userInfo.m_UserPath = usrPath;
    analysisJson(jObj, userInfo);
    return true;
}

void FileOperation::analysisJson(QJsonObject &rootObj,UserInfo &userInfo){
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
        }else {
            userInfo.m_PronunciationWeb = value;
        }
    }
    userInfo.m_Symbols = rootObj["Symbols"].toString();
}

bool FileOperation::appendWord(WordInfo wordInfo){
    QString words = wordInfo.m_Word;
    QFile file(wordPath_);
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "无法打开文件进行追加操作。";
        return false;
    }
    QTextStream stream(&file);
    stream << "\n<p><a href=https://dictionary.cambridge.org/dictionary/english-chinese-simplified/"
           << wordInfo.m_Word << ">" << wordInfo.m_Word << "</a> &nbsp; &nbsp;/" << wordInfo.m_Phonetic_UK
           << "/ &nbsp; &nbsp; <audio src=http://dict.youdao.com/dictvoice?type=0&audio=" << wordInfo.m_Word
           << " style=\"width:101px; height: 12px\"></audio> </p> \n";
    for(int i = 0; i < wordInfo.m_Translation.size(); ++i) {
        stream << "- " << wordInfo.m_Translation.at(i) << "\n";
    }

    file.close();
    qDebug() << "文本已成功追加到文件中。";
    return true;
}

bool FileOperation::appendSentence(WordInfo sentence){
    QFile file(wordPath_);
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "无法打开文件进行追加操作。";
        return false;
    }
    QTextStream stream(&file);
    QString cambridge = sentence.m_Word;
    cambridge = cambridge.replace(" ","+");

    stream <<"\n<p><a href=https://dictionary.cambridge.org/spellcheck/english-chinese-simplified/?q=" << cambridge
           << ">" << sentence.m_Word << "</a> &nbsp; &nbsp;  &nbsp; &nbsp; <audio src=http://dict.youdao.com/dictvoice?type=0&audio="
           << QUrl::toPercentEncoding(sentence.m_Word) << " style=\"width:101px; height: 12px\"></audio> </p> \n";

    stream <<"- "<< sentence.m_Translation.first() << "\n";
    file.close();
    qDebug() << "文本已成功追加到文件中。";
    return true;
}




