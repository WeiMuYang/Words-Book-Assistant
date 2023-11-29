#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDesktopWidget>
#include "data_type.h"

int printscreeninfo()
{
    QDesktopWidget *dwsktopwidget = QApplication::desktop();
    QRect deskrect = dwsktopwidget->availableGeometry();
    QRect screenrect = dwsktopwidget->screenGeometry();
    return deskrect.width();
}

double multiple = 2;  // 1 or 12/9


MainWindow::MainWindow(QWidget *parent,QApplication* app)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFont font;
    multiple_ = 2;
    screenWidth_ = printscreeninfo();
    // 1. 设置界面风格
    initScreenResNormal();
    setWindowStyle();
    // 2. 获取配置文件
    operateFile_ = new FileOperation(this);
    initConfInfo();

    initStatusBar();

    // 3. 添加 Ctrl + C 的过滤
    winEventFilter_->setSymbols(userInfo_.m_Symbols);
    app->installNativeEventFilter(winEventFilter_ = new WinEventFilter);
    connect(winEventFilter_,&WinEventFilter::sendWords,this, &MainWindow::getWordsSlot);

    netWorkAccess_ = new NetworkAccess(this);
    netWorkAccess_->setTranslateWeb(userInfo_.m_TranslateWeb);
    connect(netWorkAccess_,&NetworkAccess::sendWordInfo,this, &MainWindow::addWordListSlot);
    connect(netWorkAccess_,&NetworkAccess::sendSentenceInfo,this, &MainWindow::addSentenceListSlot);

    aboutDlg_ = new AboutDialog(this);
    openExPro_ = new OpenExProgram;
    openExPro_->setTyporaPath(userInfo_.m_TyporaPath);
    openExPro_->setVscodePath(userInfo_.m_VsCodePath);
    initActions();


    initRepoAndFilePathCombox();
    // Combox
    connect(ui->repoPathComBox,&QComboBox::currentTextChanged,this,&MainWindow::setRepoPathSlot);
    connect(ui->subPathComBox,&QComboBox::currentTextChanged,this,&MainWindow::setSubPathSlot);

    // 鼠标掠过事件
    ui->addList->installEventFilter(this);
    ui->addList->setMouseTracking(true);
    connect(ui->addList,&QListWidget::itemEntered,this, &MainWindow::itemEnteredSlot);


}

void MainWindow::initConfInfo() {
    QString iniPath, confDirPath;
    getConfPath(iniPath, confDirPath);
    if(operateFile_->readIniFile(iniPath, iniInfo_)){
        if(!operateFile_->readUserFile(confDirPath + "/" + iniInfo_.m_HostName + ".json", userInfo_)){
            appendTextToLog("读取用户文件失败");
        }
    }else {
        appendTextToLog("读取系统文件失败");
    }
}

void MainWindow::initActions() {
    connect(ui->actionAbout,&QAction::triggered, [&](){ aboutDlg_->show(); });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWordSentList() {
    ui->addList->setViewMode(QListView::ListMode);
    ui->addList->setIconSize(QSize(40 ,20 * multiple_));
    ui->addList->setSpacing(4);
}

void MainWindow::updateListWgt()
{
    ui->addList->clear();

    for(int i = 0; i < wordSentList_.size(); ++i){
        QListWidgetItem *pItem = new QListWidgetItem;
        QString data = wordSentList_.at(i).m_WordSent;
        pItem->setSizeHint(QSize(20,20 * multiple_));
        pItem->setText(data);
        pItem->setToolTip(data);

        ui->addList->addItem(pItem);
    }
}

void MainWindow::getWordsSlot(WordsType status, QString words)
{
    switch (status) {
    case IsWord:
        netWorkAccess_->accessWord(words);
        break;
    case IsSentence:
        netWorkAccess_->accessSentence(words);
        break;
    default:
        break;
    }
}

bool MainWindow::addWordSent2List(WordSentInfo wordInfo){
    for(int i = 0; i < wordSentList_.size(); ++i) {
        if(wordSentList_.at(i).m_WordSent == wordInfo.m_WordSent){
            appendTextToLog(wordInfo.m_WordSent + u8" 已存在 !");
            return false;
        }
    }
    wordSentList_.append(wordInfo);
    return true;
}

void MainWindow::addWordListSlot(WordSentInfo wordInfo){
    if(!addWordSent2List(wordInfo)) {
        return;
    }
    QListWidgetItem *pItem = new QListWidgetItem;
    QString data = wordInfo.m_WordSent;
    pItem->setSizeHint(QSize(20,20 * multiple_));
    pItem->setText(data);
    pItem->setToolTip(data);

    ui->addList->addItem(pItem);
}


void MainWindow::addSentenceListSlot(WordSentInfo Sentence) {
    if(!addWordSent2List(Sentence))
        return;
    QListWidgetItem *pItem = new QListWidgetItem;
    QString data = Sentence.m_WordSent;
    pItem->setSizeHint(QSize(20,20 * multiple_));
    pItem->setText(data);
    pItem->setToolTip(data);

    ui->addList->addItem(pItem);
}

void MainWindow::appendWordInfoSlot(WordSentInfo wordInfo) {

    if(operateFile_->appendWord(wordInfo)) {
        qDebug() << "append Word succeed ";
    }else{
        qDebug() << "append Word fauiled ";
    }
}

void MainWindow::appendSentenceInfoSlot(WordSentInfo wordInfo) {

    if(operateFile_->appendSentence(wordInfo)) {
        qDebug() << "append Sentence succeed ";
    }else{
        qDebug() << "append Sentence fauiled ";
    }
}

void MainWindow::setWindowStyle()
{
    QFont font = this->font();
    font.setPointSize(13);
    ui->addList->setFont(font);
    QFile file(":/qss/psblack.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

bool MainWindow::getConfPath(QString& iniPath, QString& confDirPath) {
    QString iniFileName = "conf.ini";
    QString iniFileDir = "conf";
    QString dotPath = QDir::currentPath();;
    QDir dir1(dotPath);
    while(!dir1.exists(iniFileDir) && !dir1.isRoot()){
        dir1.cdUp();
    }
    if(dir1.exists(iniFileDir)){
        dir1.cd(iniFileDir);
        if(dir1.exists(iniFileName)){
            confDirPath =dir1.absolutePath();
            iniPath = dir1.absolutePath()+"/"+iniFileName;
            return true;
        }
    }
    return false;
}
void MainWindow::appendTextToLog(QString log)
{
    ui->logText->append(log.toUtf8());
}

void MainWindow::itemEnteredSlot(QListWidgetItem *item)
{
    QString name = item->text();
    WordSentInfo wordInfo;
    for (int i = 0; i < wordSentList_.size(); ++i) {
        if(wordSentList_.at(i).m_WordSent == name) {
            wordInfo = wordSentList_.at(i);
            break;
        }
    }
    for(int i = 0; i < wordSentList_.size(); ++i ) {
        if(ui->addList->item(i)->text() == name){
            ui->textEdit->clear();
            if(-1 == wordInfo.m_WordSent.indexOf(" ")) {
                ui->textEdit->append("<font color=\"#00FFFF\">"
                                     + wordInfo.m_WordSent + "</font> /"
                                     + wordInfo.m_Phonetic_UK +"/");
            } else {
                ui->textEdit->append("<font color=\"#00FFFF\">"
                                     + wordInfo.m_WordSent + "</font>");
            }
            for(int i = 0; i < wordInfo.m_Translation.size(); ++i) {
                ui->textEdit->append("- " + wordInfo.m_Translation.at(i));
            }
            break;
        }
    }
}

ScreenRes MainWindow::getScrrenRes()
{
    if(screenWidth_ >= 3840){
        return ScreenRes::High;
    }else{
        return ScreenRes::Low;
    }
}

void MainWindow::setNormalViewByScreenRes(){
    if(getScrrenRes() == ScreenRes::High){
        this->setMinimumSize(QSize(1100, 1000));
        this->resize(QSize(1100, 1000));
        ui->pathWgt->setMinimumSize(QSize(500,155));
        multiple = 2;
    }else{
        this->setMinimumSize(QSize(860, 600)); // int w, int h
        this->resize(QSize(860, 600));
        ui->pathWgt->setMaximumHeight(126);
        ui->pathWgt->setMinimumSize(QSize(0,126));
        ui->pathWgt->setMinimumWidth(440);
        multiple = 1;
    }
}

void MainWindow::initScreenResNormal()
{
    setNormalViewByScreenRes();
    initListWgt();
}

void MainWindow::initListWgt(){
    QFont font;
    if(getScrrenRes() == ScreenRes::High){
        font = ui->addList->font();
        font.setPointSize(13);
        ui->addList->setFont(font);
        multiple = 2;
    }else{
        font = ui->addList->font();
        font.setPointSize(10);
        ui->addList->setFont(font);
        multiple = 1;
    }
}

// 1 Init
void MainWindow::initRepoAndFilePathCombox()
{
    for(int i = 0; i < userInfo_.m_RepoPathList.size(); ++i) {
        ui->repoPathComBox->addItem(userInfo_.m_RepoPathList.at(i).m_Name);
    }

    repoPathName_ = ui->repoPathComBox->currentText();
    updateSubDirCombox();
}

QString MainWindow::getRepoPathByName(QString name){
    for(int i = 0; i < userInfo_.m_RepoPathList.size(); ++i){
        if(userInfo_.m_RepoPathList.at(i).m_Name == name) {
            return userInfo_.m_RepoPathList.at(i).m_Path;
        }
    }
    return QString("");
}


void MainWindow::updateSubDirCombox(){
    ui->subPathComBox->clear();
    QString repoPath = getRepoPathByName(repoPathName_);
    if(!repoPath.isEmpty()){
        ui->subPathComBox->addItems(operateFile_->getSubDirNames(repoPath));
        subDirName_ = ui->subPathComBox->currentText();
        whoIsBoxSelection(BoxSelect::SubCombox);
        setSubPathSlot(ui->subPathComBox->currentText());
    }else{
        appendTextToLog("repoPathName error!");
    }
}

void MainWindow::setRepoPathSlot(QString currentStr){
    repoPathName_ = currentStr;
    updateSubDirCombox();
    whoIsBoxSelection(BoxSelect::SubCombox);
    //    if(tarPath_.isEmpty() || !fileOp_.isPathExist(this->tarPath_)){
    //        ui->subPathComBox->clear();
    //        fullTarPath_.clear();
    //        subDirName_.clear();
    //        tarPath_.clear();
    //        ui->numSpinBox->setValue(-1);
    //        appendTextToLog(QString(u8"当前的目标路径不存在 !"));
    //        currentFile_.clear();
    //        setStatusBar("", false);
    //        whoIsBoxSelection(BoxSelect::None);
    //        return;
    //    }
    //    whoIsBoxSelection(BoxSelect::TarCombox);
    //    updateSubDirCombox();
    //    updateLastModifyFile();
    //    updateRepoHistoryFileList();
}

void MainWindow::setSubPathSlot(QString currentStr)
{
    if(currentStr.isEmpty()){
        return;
    }
    subDirName_ = currentStr;
    QString repoPath =this->getRepoPathByName(repoPathName_);
    int num = operateFile_->getLastmodifiedTimeFileNumSubDir(repoPath ,subDirName_, currentFile_);
    if(num == -1){
        ui->numSpinBox->setValue(num);
        appendTextToLog(QString(u8"当前的目标路径不存在 !"));
        setStatusBar("", false);
        whoIsBoxSelection(BoxSelect::SubCombox);
    }else{
        ui->numSpinBox->setValue(num);
        setStatusBar("", true);
        whoIsBoxSelection(BoxSelect::NumSpinBox);
    }
}

void MainWindow::whoIsBoxSelection(BoxSelect select)
{
    boxSelect_ = select;
    switch (select) {
    case BoxSelect::SubCombox:
        changSubPathStyle(true);
        changNumStyle(false);
        break;
    case BoxSelect::NumSpinBox:
        changSubPathStyle(false);
        changNumStyle(true);
        break;

    default:  // None
        changSubPathStyle(false);
        changNumStyle(false);
        break;
    }
}
void MainWindow::changNumStyle(bool flags){
    if(flags){
        ui->numSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}\nQSpinBox{\nborder: 1px solid #d9d5d2\n}\n");
    }else{
        ui->numSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}");
    }
    QFont font;
    font.setPointSize(12);
    ui->numSpinBox->setFont(font);
}

void MainWindow::changSubPathStyle(bool flags){
    if(flags){
        ui->subPathComBox->setStyleSheet("border: 1px solid #d9d5d2");
    }else{
        ui->subPathComBox->setStyleSheet("");
    }
    QFont font;
    font.setPointSize(12);
    ui->subPathComBox->setFont(font);
}

void MainWindow::on_numSpinBox_valueChanged(int num)
{
    if(num == -1){
        whoIsBoxSelection(BoxSelect::SubCombox);
        return;
    }
    QString repoPath = getRepoPathByName(repoPathName_);
    if(repoPath.isEmpty()){
        appendTextToLog(u8"当前的目标路径不存在 !");
        whoIsBoxSelection(BoxSelect::SubCombox);
        return;
    }
    if(operateFile_->getFileNameByNum(repoPath+"/" + subDirName_, num, currentFile_)){
        if(currentFile_.size() > 3 && currentFile_.right(2) == "md"){
            appendTextToLog(u8"当前文档为：" + currentFile_);
        }else{
            appendTextToLog(u8"当前目录为：" + currentFile_);
        }
        setStatusBar("",true);
        whoIsBoxSelection(BoxSelect::NumSpinBox);
    }else{
        setStatusBar("",false);
        currentFile_.clear();
        whoIsBoxSelection(BoxSelect::SubCombox);

    }
}

void MainWindow::setStatusBar(QString msg, bool isCorrect){
    Q_UNUSED(msg)
    if(isCorrect){
        QImage image(QString(":/qss/psblack/checkbox_checked.png"));
        pStatusLabelIcon_->setPixmap(QPixmap::fromImage(image));
        pStatusLabelIcon_->setMinimumWidth(15);
        pStatusLabelMsg_->setText(u8"正常");
        pStatusLabelCurrentFile_->setText("|  " +subDirName_ +"/"+ currentFile_);
    }else{
        QImage image(QString(":/qss/psblack/checkbox_checked_disable.png"));
        pStatusLabelIcon_->setPixmap(QPixmap::fromImage(image));
        pStatusLabelIcon_->setMinimumWidth(15);
        pStatusLabelMsg_->setText(u8"错误");
        pStatusLabelCurrentFile_->setText("|  " + currentFile_);
    }

    ui->statusBar->addWidget(pStatusLabelIcon_);
    ui->statusBar->addWidget(pStatusLabelMsg_);
    ui->statusBar->addWidget(pStatusLabelCurrentFile_);
}

void MainWindow::initStatusBar(){
    pStatusLabelIcon_ = new QLabel("",this);
    pStatusLabelMsg_ = new QLabel("",this);
    pStatusLabelMsg_->setAlignment(Qt::AlignLeft);
    pStatusLabelCurrentFile_ = new QLabel("",this);
    pStatusLabelCurrentFile_->setAlignment(Qt::AlignHCenter);
}

void MainWindow::on_toolButton_clicked()
{
    openExPro_->OpenDirSlot(getRepoPathByName(repoPathName_));
}

void MainWindow::on_openFilePbn_clicked()
{
    QString repoPath = getRepoPathByName(repoPathName_);
    openExPro_->OpenMarkdownAndDirSlot(repoPath + "/" + repoPath + "/" + subDirName_ + "/" + currentFile_);
}


void MainWindow::on_addWordSentPbn_clicked()
{

}

