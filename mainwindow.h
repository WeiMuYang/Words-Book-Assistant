#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTextStream>
#include "win_event_filter.h"
#include "file_operation.h"
#include "network_access.h"
#include "about_dialog.h"
#include "open_ex_program.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr,QApplication *app = nullptr);
    ~MainWindow();
    void setWindowStyle();
    bool getConfPath(QString& iniPath, QString& confDirPath);
    void initConfInfo();
    void initWordSentList();
    void updateListWgt();

public slots:
    void getWordsSlot(WordsType status, QString words);
    void appendWordInfoSlot(WordSentInfo wordInfo);
    void appendSentenceInfoSlot(WordSentInfo wordInfo);
    void initActions();
    void appendTextToLog(QString log);
    void addWordListSlot(WordSentInfo wordInfo);
    void itemEnteredSlot(QListWidgetItem *item);
    void addSentenceListSlot(WordSentInfo Sentence);
private:
    Ui::MainWindow *ui;
    WinEventFilter *winEventFilter_;
    FileOperation *operateFile_;
    NetworkAccess *netWorkAccess_;
    AboutDialog *aboutDlg_;
    OpenExProgram *openExPro_;
    IniInfo iniInfo_;
    UserInfo userInfo_;
    QVector<WordSentInfo> wordSentList_;
    int multiple_;

};
#endif // MAINWINDOW_H
