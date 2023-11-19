#ifndef WINEVENTFILTER_H
#define WINEVENTFILTER_H

#include <QObject>
#include <windows.h>
#include <QAbstractNativeEventFilter>

enum WordsType{
 IsWord, IsSentence
};

class WinEventFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit WinEventFilter(QObject * parent = 0) : QObject(parent) {
        // 添加小写字母
        for (char c = 'a'; c <= 'z'; ++c) {
            letters_.append(c);
        }
        // 添加大写字母
        for (char c = 'A'; c <= 'Z'; ++c) {
            letters_.append(c);
        }
    }

public slots:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

    QString getCliptext(){
        return clipText_;
    }

    bool getWords(WordsType& w);
signals:
    void sendWords(WordsType status, QString words);

private:
    QString clipText_;
     QString letters_;

};

#endif // WINEVENTFILTER_H
