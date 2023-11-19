#include "operate_file.h"
#include <QFile>
#include <QDebug>


OperateFile::OperateFile(QObject *parent)
    : QObject{parent}
{
    wordPath_ = "C:\\Users\\Administrator\\Desktop\\words-doc.md";
    website_ = "https://dictionary.cambridge.org/dictionary/english-chinese-simplified/";
    audio_ = "http://dict.youdao.com/dictvoice?type=0&audio=";
}

bool OperateFile::appendWord(QString words){
    QFile file(wordPath_);
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "无法打开文件进行追加操作。";
        return false;
    }
    QTextStream stream(&file);
    stream <<"\n" <<"- "<< "[" << words << "](" << website_ + words <<") " ;
    stream << "\n<center>\n<audio src=" << audio_ << words << "></audio>\n</center>";

    file.close();
    qDebug() << "文本已成功追加到文件中。";
    return true;
}

bool OperateFile::appendSentence( QString words){
    QFile file(wordPath_);
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "无法打开文件进行追加操作。";
        return false;
    }
    QTextStream stream(&file);
    stream <<"\n" <<"- "<< words <<" " ;
    file.close();
    qDebug() << "文本已成功追加到文件中。";
    return true;
}
