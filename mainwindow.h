#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

public slots:
    void getWordsSlot(WordsType status, QString words);
    void appendWordInfoSlot(WordInfo wordInfo);
    void appendSentenceInfoSlot(WordInfo wordInfo);
    void initActions();
    void appendTextToLog(QString log);

private:
    Ui::MainWindow *ui;
    WinEventFilter *winEventFilter_;
    FileOperation *operateFile_;
    NetworkAccess *netWorkAccess_;
    AboutDialog *aboutDlg_;
    OpenExProgram *openExPro_;
    IniInfo iniInfo_;
    UserInfo userInfo_;
};
#endif // MAINWINDOW_H
