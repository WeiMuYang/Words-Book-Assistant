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
    explicit WinEventFilter(QObject * parent = 0) ;
    void setSymbols(QString p);

public slots:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;

    QString getCliptext(){
        return clipText_;
    }

    bool getWords(WordsType& w);
signals:
    void sendWords(WordsType status, QString words);
    void sigWinEvenFilterLog(QString msg);

private:
    QString clipText_;
    QString letters_;
    QString symbols_;
};

#endif // WINEVENTFILTER_H
