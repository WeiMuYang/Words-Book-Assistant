#include "file_operation.h"
#include <QFile>
#include <QDebug>
#include <QRegExp>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include "data_type.h"

FileOperation::FileOperation(QObject *parent)
    : QObject{parent}
{
//    cambridgeWordWeb_ = "https://dictionary.cambridge.org/dictionary/english-chinese-simplified/";
//    pronunciationWeb_ = "http://dict.youdao.com/dictvoice?type=0&audio=";
}

bool isMarkdownFile(QString fileName){
    if(fileName.size() >= 5 && fileName.right(3) == ".md"){
        return true;
    }
    return false;
}


bool FileOperation::appendWord(const QString& path, WordSentInfo wordInfo, const QString& cambridgeWordWeb_,
                               const QString& pronunciationWeb_){
    QString words = wordInfo.m_WordSent;
    QFile file(path);
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        emit sigFileOperationLog("无法打开文件进行添加操作!");
        return false;
    }
    QTextStream stream(&file);
    if(!wordInfo.m_Phonetic_UK.isEmpty()) {
        stream << "\n<p><a href=" << cambridgeWordWeb_
               << wordInfo.m_WordSent << ">" << wordInfo.m_WordSent << "</a> &nbsp; &nbsp;/" << wordInfo.m_Phonetic_UK
               << "/ &nbsp; &nbsp; <audio src=" << pronunciationWeb_ << wordInfo.m_WordSent
               << " style=\"width:101px; height: 12px\"></audio> </p> \n";
    }else {
        stream << "\n<p><a href=" << cambridgeWordWeb_
               << wordInfo.m_WordSent << ">" << wordInfo.m_WordSent << "</a> &nbsp; &nbsp;" << wordInfo.m_Phonetic_UK
               << " &nbsp; &nbsp; <audio src=" << pronunciationWeb_ << wordInfo.m_WordSent
               << " style=\"width:101px; height: 12px\"></audio> </p> \n";
    }

    for(int i = 0; i < wordInfo.m_Translation.size(); ++i) {
        stream << "- " << wordInfo.m_Translation.at(i) << "\n";
    }

    file.close();
    //    emit sigFileOperationLog("单词已成功添加到文件中!");
    return true;
}

bool FileOperation::appendSentence(const QString& path,WordSentInfo sentence, const QString& cambridgeSentWeb_,
                                   const QString& pronunciationWeb_){
    QFile file(path);
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        emit sigFileOperationLog("无法打开文件进行添加操作。");
        return false;
    }
    QTextStream stream(&file);
    QString replaceSpaceSent = sentence.m_WordSent;
    replaceSpaceSent = replaceSpaceSent.replace(" ","+");

    stream <<"\n<p><a href=" << cambridgeSentWeb_ << replaceSpaceSent
          << ">" << sentence.m_WordSent << "</a> &nbsp; &nbsp;  &nbsp; &nbsp; <audio src=" << pronunciationWeb_
          << QUrl::toPercentEncoding(sentence.m_WordSent) << " style=\"width:101px; height: 12px\"></audio> </p> \n";
    stream <<"- "<< sentence.m_Translation.first() << "\n";
    file.close();
    //    emit sigFileOperationLog("句子已成功追加到文件中!");
    return true;
}

QStringList FileOperation::getSubDirNames(QString TarPath){
    QStringList dirList;
    QDir Root(TarPath);
    QFileInfoList fileList = Root.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Time);
    for(auto it = fileList.begin(); it < fileList.end(); ++it){
        dirList << it->fileName();
    }
    return dirList;
}

bool FileOperation::getFileNameByNum(QString fullPath, int fileNum, QString& fileName){
    QDir dir(fullPath);
    QStringList filters;
    QString nameNum = QString("%1").arg(fileNum, 2, 10, QLatin1Char('0'))+"*";
    filters << nameNum;
    dir.setFilter(QDir::AllEntries);
    dir.setNameFilters(filters);
    QFileInfoList fileInfoList = dir.entryInfoList();
    if(fileInfoList.size() > 1) {
        QString msg;
        msg = QString("包含") + QString::number(fileInfoList.size())+ QString("个序号为： \"") + QString("%1").arg(fileNum, 2, 10, QLatin1Char('0')) + QString("\" 的markdown文件：\n");
        for(auto it = fileInfoList.begin(); it < fileInfoList.end(); it++){
            msg += it->fileName() + ", ";
        }
        msg[msg.size()-2] = '!';
        emit sigFileOperationLog(msg);
    }else if(fileInfoList.size() < 1){
        QString msg;
        msg = QString("序号为：\"") + QString("%1").arg(fileNum, 2, 10, QLatin1Char('0')) + QString("\"的文件or目录不存在！").toUtf8();
        emit sigFileOperationLog(msg);
        return false;
    }

    fileName = fileInfoList.last().fileName();
    //    emit sigFileOperationLog(QString("当前文档/目录为：")+fileName);
    return true;
}

int FileOperation::getLastmodifiedTimeFileNumSubDir(const QString &path,const QString &dirName,QString& lastModefyFile)
{
    int num;
    QDir dir(path+"/"+dirName);
    QStringList filters;
    filters << "*.md"<< "\?\?-*";
    dir.setNameFilters(filters);
    QFileInfoList list = dir.entryInfoList(QDir::AllEntries, QDir::Time);
    if(list.isEmpty()){
        emit sigFileOperationLog("Directory: \"" + path + dirName + "\" does not exist files!");
        return -1;
    }
    lastModefyFile = list.first().fileName();
    QStringList nameArr = lastModefyFile.split("-");
    num = nameArr.at(0).toInt();
    return num;
}

bool FileOperation::createMarkdownFile(const QString& FullPath, QString& currentFileName){
    QDir dir(FullPath);
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    int max = -1;
    QString templateFileName;
    for(int i = 0; i < list.size(); ++i){
        if(isMarkdownFile(list.at(i).fileName())){
            QStringList nameArr = list.at(i).fileName().split("-");
            int num = nameArr.at(0).toInt();
            if(num > max){
                max = num;
            }
            if(num == 0 && list.at(i).fileName().right(5) != "um.md"){
                templateFileName =list.at(i).fileName();
            }
        }
    }
    int num = max+1;
    QString fileName = QString("%1").arg(num, 2, 10, QLatin1Char('0')) + "-新建文件.md";
    QString path =FullPath + "/" + fileName;

    if(!templateFileName.isEmpty()){
        QString templateFile = FullPath + "/" + templateFileName;
        if(!QFile::copy(templateFile, path))
        {
            emit sigFileOperationLog(templateFile + QString(" copy failed!"));
        }
        emit sigFileOperationLog(QString("Copy: "+templateFile+"\nTo Create:"+ path + "\nCreate File Success  !!!"));
        // 将拷贝的文件追加回车，成为最近修改文件
        QFile file(path);
        file.open(QIODevice::ReadWrite | QIODevice::Append);
        QTextStream txtOutput(&file);
        txtOutput << "\n";
        file.close();
        currentFileName = fileName;
    }else{
        QFile file(path);
        if (file.exists()) {
            emit sigFileOperationLog(QString("新建文件已存在！"));
            return false;
        }
        file.open(QIODevice::WriteOnly);
        QString text =  "# [新建文件](./)    \n" \
                        "\n" \
                        "> ######  _标签:_   ![](https://img.shields.io/badge/英语类-yellowgreen.svg)   ![](https://img.shields.io/badge/IELTS-雅思考试-blue.svg)   [![](https://img.shields.io/badge/链接-有道词典-brightgreen.svg)](https://dict.youdao.com/?keyfrom=cidian)   [![](https://img.shields.io/badge/链接-Cambridge-orange.svg)](https://dictionary.cambridge.org/)   \n"   \
                        ">  \n\n\n";
        QByteArray str = text.toUtf8();
        file.write(str);
        emit sigFileOperationLog(QString("Create File") + path + "\nCreate File Success");
        currentFileName = fileName;
        file.close();
    }
    return true;
}

int FileOperation::getCurrentFileWordNum(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file";
        return -1;
    }

    // 读取文件内容并统计字符串个数
    QTextStream in(&file);
    QString content = in.readAll();
    int count = content.count("http://dict.youdao.com");

    // 关闭文件
    file.close();
    return count;
}

bool FileOperation::getCurrentFileWordList(const QString &path, QStringList & list)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file";
        return false;
    }

    // 读取文件内容并统计字符串个数
    QTextStream in(&file);
    QString content = in.readAll();
    //    english-chinese-simplified/elastic>elastic</a>
    QStringList slist = content.split("</a>");
    for(int i = 0; i < slist.size() - 1 ; ++i) {
        list.append(slist.at(i).split(">").last());
    }

    // 关闭文件
    file.close();
    return true;
}

