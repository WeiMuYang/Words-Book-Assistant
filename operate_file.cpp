#include "operate_file.h"
#include <QFile>
#include <QDebug>
#include <QRegExp>
#include <QUrl>
#include "data_type.h"

OperateFile::OperateFile(QObject *parent)
    : QObject{parent}
{
    wordPath_ = "../words-doc.md";
    website_ = "https://dictionary.cambridge.org/dictionary/english-chinese-simplified/";
    audio_ = "http://dict.youdao.com/dictvoice?type=0&audio=";
}

bool OperateFile::appendWord(WordInfo wordInfo){
    QString words = wordInfo.m_word;
    QFile file(wordPath_);
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "无法打开文件进行追加操作。";
        return false;
    }
    QTextStream stream(&file);
    stream << "\n<p><a href=https://dictionary.cambridge.org/dictionary/english-chinese-simplified/"
           << wordInfo.m_word << ">" << wordInfo.m_word << "</a> &nbsp; &nbsp;/" << wordInfo.m_Phonetic_UK
           << "/ &nbsp; &nbsp; <audio src=http://dict.youdao.com/dictvoice?type=0&audio=" << wordInfo.m_word
           << " style=\"width:101px; height: 12px\"></audio> </p> \n";
    for(int i = 0; i < wordInfo.m_Translation.size(); ++i) {
        stream << "- " << wordInfo.m_Translation.at(i) << "\n";
    }

    file.close();
    qDebug() << "文本已成功追加到文件中。";
    return true;
}

bool OperateFile::appendSentence(WordInfo sentence){
    QFile file(wordPath_);
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "无法打开文件进行追加操作。";
        return false;
    }
    QTextStream stream(&file);
    QString cambridge = sentence.m_word;
    cambridge = cambridge.replace(" ","+");

    stream <<"\n<p><a href=https://dictionary.cambridge.org/spellcheck/english-chinese-simplified/?q=" << cambridge
           << ">" << sentence.m_word << "</a> &nbsp; &nbsp;  &nbsp; &nbsp; <audio src=http://dict.youdao.com/dictvoice?type=0&audio="
           << QUrl::toPercentEncoding(sentence.m_word) << " style=\"width:101px; height: 12px\"></audio> </p> \n";

    stream <<"- "<< sentence.m_Translation.first() << "\n";
    file.close();
    qDebug() << "文本已成功追加到文件中。";
    return true;
}
