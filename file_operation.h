#ifndef OPERATEFILE_H
#define OPERATEFILE_H

#include <QObject>
#include "data_type.h"
class FileOperation : public QObject
{
    Q_OBJECT
public:
    explicit FileOperation(QObject *parent = nullptr);

    QStringList getSubDirNames(QString TarPath);
    bool getFileNameByNum(QString fullPath, int fileNum, QString& fileName);
    int getLastmodifiedTimeFileNumSubDir(const QString &path,const QString &dirName,QString& lastModefyFile);
    bool createMarkdownFile(const QString& FullPath, QString& currentFileName);
    int getCurrentFileWordNum(const QString& path);
    bool getCurrentFileWordList(const QString &path, QStringList& list);

public slots:
    bool appendWord(const QString& path, WordSentInfo wordInfo, const QString& cambridgeWordWeb_,
                                   const QString& pronunciationWeb_);
    bool appendSentence(const QString& path,WordSentInfo sentence, const QString& cambridgeSentWeb_,
                                       const QString& pronunciationWeb_);

signals:
    void sigFileOperationLog(QString m);

private:
//    QString cambridgeWordWeb_;
//    QString cambridgeSentWeb_;
//    QString pronunciationWeb_;
};

#endif // OPERATEFILE_H
