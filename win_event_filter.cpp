#include "win_event_filter.h"
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QRegularExpression>

//QString punctuation = QString::fromUtf8(" !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");
QString punctuation = QString::fromUtf8(" !\"'()*,-.:;<>?[]_|~");


bool WinEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result){
    Q_UNUSED(result)
    if(eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        // 目前调查结果: Ctrl + C会触发windows_generic_MSG的全局快捷键，
        // 且msg->message == 797 需要在不同电脑上验证
        if(msg->message == 797){
            QClipboard *clipboard = QApplication::clipboard();
            // 获取剪贴板内容
            clipText_ = clipboard->text();
            // 去掉前后空格
            clipText_ = clipText_.trimmed();
            qDebug() << clipText_;
            WordsType type;
            if(getWords(type))
                emit sendWords(type, clipText_);
        }
    }
    return false;
}

bool WinEventFilter::getWords(WordsType& type){
    bool isWordOrSentence = true;
    type = IsWord;
    for(int i = 0; i < clipText_.length(); i++){
        if(!clipText_[i].isLower() && !clipText_[i].isUpper()) {
            type = IsSentence;
            if(punctuation.indexOf(clipText_[i]) == -1) {
                isWordOrSentence = false;
                break;
            }
        }
    }
    if(!isWordOrSentence){
        return false;
    }
    return true;
}
