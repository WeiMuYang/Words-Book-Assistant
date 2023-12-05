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

    QStringList getSubDirNames(QString TarPath);
    bool getFileNameByNum(QString fullPath, int fileNum, QString& fileName);
    int getLastmodifiedTimeFileNumSubDir(const QString &path,const QString &dirName,QString& lastModefyFile);
    bool createMarkdownFile(const QString& FullPath, QString& currentFileName);
    int getCurrentFileWordNum(const QString& path);
    bool getCurrentFileWordList(const QString &path, QStringList& list);

public slots:
    bool appendWord(const QString& path,WordSentInfo wordInfo);
    bool appendSentence(const QString& path, WordSentInfo sentence);

signals:
    void sigFileOperationLog(QString m);

private:
    QString cambridgeWordWeb_;
    QString cambridgeSentWeb_;
    QString pronunciationWeb_;
};

#endif // OPERATEFILE_H
