#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "win_event_filter.h"
#include "operate_file.h"
#include "network_access.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr,QApplication *app = nullptr);
    ~MainWindow();

public slots:
    void getWordsSlot(WordsType status, QString words);

private:
    Ui::MainWindow *ui;
    WinEventFilter *winEventFilter_;
    OperateFile *operateFile_;
    NetworkAccess *netWorkAccess_;
};
#endif // MAINWINDOW_H
