#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTextStream>
#include <QLabel>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include "win_event_filter.h"
#include "file_operation.h"
#include "network_access.h"
#include "about_dialog.h"
#include "open_ex_program.h"
#include "create_markdown_dir.h"
#include "conf_file_operate.h"

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
    void initAddListMenu();
    void initWordSentList();
    void initStatusBar();
    void updateListWgt();
    ScreenRes getScrrenRes();
    void setNormalViewByScreenRes();
    void initScreenResNormal();
    void initListWgt();
    void initRepoAndFilePathCombox();
    QString getRepoPathByName(QString name);
    void updateSubDirCombox();
    void whoIsBoxSelection(BoxSelect select);
    void changNumStyle(bool flags);
    void changSubPathStyle(bool flags);
    void changRepoComStyle(bool flags);

    void updateAddListWgt();
    void delWordListDataByName(QString name);

    void setSampleView();
    void setNormalView();
    void setSampleViewByScreenRes();
    bool isWord(QString text);
    bool appendWordSentList(QString s);
    bool isNotAtMarkdown(QString name);
    void initTray();
public slots:
    void getWordsSlot(WordsType status, QString words);
    void initActions();
    void appendTextToLog(QString log);
    void addWordListSlot(WordSentInfo wordInfo);
    void addWord2UiListSlot(QString wordInfo);
    void itemEnteredSlot(QListWidgetItem *item);
    void addSentenceListSlot(WordSentInfo Sentence);
    void setRepoPathSlot(QString currentStr);
    void setSubPathSlot(QString currentStr);
    void numSpinBoxValueChangedSlot(int num);
    void changeSelectSatusSlot();
    void moveDelItemFromListSlot(const QModelIndex &index);
    void delItemFromAddListSlot();
    void setStatusBar(QString msg, bool isCorrect);

    void modifyIniFileSlot();
    void modifyConfSlot();
    void clearTabWgtSlot();

    void start();

    void simpleViewSlot();

    void syncAddTimelySlot();

    bool addWordSent2ListSlot(WordSentInfo wordInfo);

private slots:
    void on_toolButton_clicked();
    void on_openFilePbn_clicked();
    void on_addWordSentPbn_clicked();
    void on_createMarkdownPbn_clicked();
    void on_syncPbn_clicked();
    void quitAppSlot();
    void trayIconClickedSlot(QSystemTrayIcon::ActivationReason reason);
    void createMarkdownAndSubDirSlot(int type, QString namePathAbs);

protected:
    void closeEvent(QCloseEvent *event);

private:
    int screenWidth_;
    Ui::MainWindow *ui;
    WinEventFilter *winEventFilter_;
    FileOperation *operateFile_;
    NetworkAccess *netWorkAccess_;
    AboutDialog *aboutDlg_;
    OpenExProgram *openExPro_;
    IniInfo iniInfo_;
    UserInfo userInfo_;
    QVector<WordSentInfo> wordSentListInfo_;
    QStringList wordSentList_;
    int multiple_;

    QString repoPathName_;
    QString subDirName_;
    QString currentFile_;
    BoxSelect boxSelect_;

    bool isStayTop_{false};
    int simpleViewNum_{0};

    QLabel* pStatusLabelIcon_;
    QLabel* pStatusLabelMsg_;
    QLabel* pStatusLabelCurrentFile_;
    QLabel* pStatusLabelWordCount_;

    QTimer *timerSync_;
    bool isSyncStart_{false};


    int currentWordCount_;
    QStringList currentWordList_;

    QSystemTrayIcon *trayIcon_;
    QMenu *trayMenu_;

    CreateMarkdownAndSubDir* createMarkdownAndSubDirDlg_;
    ConfFileOperation* confFileOp_;

};
#endif // MAINWINDOW_H
