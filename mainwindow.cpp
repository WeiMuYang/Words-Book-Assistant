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
    multiple_ = 2;
    screenWidth_ = printscreeninfo();
    // 1. 设置界面风格
    initScreenResNormal();
    setWindowStyle();
    // 2. 获取配置文件
    operateFile_ = new FileOperation(this);
    aboutDlg_ = new AboutDialog(this);
    openExPro_ = new OpenExProgram;
    netWorkAccess_ = new NetworkAccess(this);
    initActions();
    initStatusBar();

    app->installNativeEventFilter(winEventFilter_ = new WinEventFilter);
    // numSpinBox
    connect(ui->numSpinBox,QOverload<int>::of(&QSpinBox::valueChanged),this,&MainWindow::numSpinBoxValueChangedSlot);
    // Combox
    connect(ui->repoPathComBox,&QComboBox::currentTextChanged,this,&MainWindow::setRepoPathSlot);
    connect(ui->subPathComBox,&QComboBox::currentTextChanged,this,&MainWindow::setSubPathSlot);
    // addList
    ui->addList->setMovement(QListWidget::Static);
    ui->addList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->addList->setContextMenuPolicy(Qt::CustomContextMenu);
    // 拖拽调整次序
    ui->addList->setDragDropMode(QAbstractItemView::InternalMove);
    ui->addList->installEventFilter(this);
    ui->addList->setMouseTracking(true);
    connect(ui->addList,&QListWidget::itemEntered,this, &MainWindow::itemEnteredSlot);
    connect(ui->addList,&QListWidget::clicked,this, &MainWindow::changeSelectSatusSlot);
    // doubleClick
    connect(ui->addList,&QListWidget::doubleClicked,this, &MainWindow::moveDelItemFromListSlot);

    timerSync_ = new QTimer(this);
    connect(timerSync_, &QTimer::timeout, this, &MainWindow::syncAddTimelySlot);
    timerSync_->setInterval(1500);

    // log
    connect(netWorkAccess_,&NetworkAccess::sigNetworkAccessLog,this, &MainWindow::appendTextToLog);
    connect(openExPro_,&OpenExProgram::sigOpenExProLog,this, &MainWindow::appendTextToLog);
    connect(winEventFilter_,&WinEventFilter::sigWinEvenFilterLog,this, &MainWindow::appendTextToLog);
    connect(operateFile_,&FileOperation::sigFileOperationLog,this, &MainWindow::appendTextToLog);

    start();
}

void MainWindow::start(){
    initConfInfo();
    // process
    openExPro_->setTyporaPath(userInfo_.m_TyporaPath);
    openExPro_->setVscodePath(userInfo_.m_VsCodePath);
    // 添加Ctrl + C的过滤
    winEventFilter_->setSymbols(userInfo_.m_Symbols);
    connect(winEventFilter_,&WinEventFilter::sendWords,this, &MainWindow::getWordsSlot);
    // network
    netWorkAccess_->setTranslateWeb(userInfo_.m_TranslateWeb);
    connect(netWorkAccess_,&NetworkAccess::sendWordInfo,this, &MainWindow::addWordListSlot);
    connect(netWorkAccess_,&NetworkAccess::sendSentenceInfo,this, &MainWindow::addSentenceListSlot);
    // repo combox
    initRepoAndFilePathCombox();
    // sub combox
    updateSubDirCombox();
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

    ui->actionDelListItem->setShortcut(QKeySequence::Delete);
    connect(ui->actionDelListItem, &QAction::triggered, this, &MainWindow::delItemFromAddListSlot);

    ui->actionRestart->setShortcut(QKeySequence("Ctrl+R"));
    connect(ui->actionRestart,&QAction::triggered, this, &MainWindow::start);

    connect(ui->actionModifySysFile, &QAction::triggered, this, &MainWindow::modifyIniFileSlot);

    ui->actionModifyConf->setShortcut(QKeySequence("Ctrl+M"));
    connect(ui->actionModifyConf, &QAction::triggered, this, &MainWindow::modifyConfSlot);

    ui->actionClearLog->setShortcut(QKeySequence::Refresh);
    ui->actionClearLog->setShortcutContext(Qt::ApplicationShortcut);
    connect(ui->actionClearLog,&QAction::triggered, this, &MainWindow::clearTabWgtSlot);

    ui->actionStayTop->setShortcut(QKeySequence("Ctrl+F1"));
    connect(ui->actionStayTop,&QAction::triggered,[this]() {
        isStayTop_ = !isStayTop_;
        if(isStayTop_) {
            setWindowFlag(Qt::WindowStaysOnTopHint, true);
            show();
        }else {
            setWindowFlag(Qt::WindowStaysOnTopHint, false);
            show();
        }
    });

    if(simpleViewNum_ % 2 != 0){
        setSampleView();
        ui->actionSimpleView->setText(u8"正常窗口");
        ui->actionSimpleView->setShortcut(QKeySequence("Ctrl+="));
    }else{
        setNormalView();
        ui->actionSimpleView->setText(u8"极简窗口");
        ui->actionSimpleView->setShortcut(QKeySequence("Ctrl+-"));
    }
    connect(ui->actionSimpleView, &QAction::triggered, this, &MainWindow::simpleViewSlot,Qt::UniqueConnection);

    // actionAutoSync
    ui->actionAutoSync->setShortcut(QKeySequence("Ctrl+T"));
    connect(ui->actionAutoSync, &QAction::triggered, this, &MainWindow::on_syncPbn_clicked);

    // actionOpenBook
    ui->actionOpenBook->setShortcut(QKeySequence("Ctrl+O"));
    connect(ui->actionOpenBook, &QAction::triggered, this, &MainWindow::on_openFilePbn_clicked);

    // actionOpenRepo
    ui->actionOpenRepo->setShortcut(QKeySequence("Ctrl+D"));
    connect(ui->actionOpenRepo, &QAction::triggered, this, &MainWindow::on_toolButton_clicked);

    // actionCreateBook
    ui->actionCreateBook->setShortcut(QKeySequence("Ctrl+N"));
    connect(ui->actionCreateBook, &QAction::triggered, this, &MainWindow::on_createMarkdownPbn_clicked);

    //actionAddWord
    ui->actionAddWord->setShortcut(QKeySequence("Ctrl+A"));
    connect(ui->actionAddWord, &QAction::triggered, this, &MainWindow::on_addWordSentPbn_clicked);
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

bool MainWindow::isWord(QString text){
    for(int i = 0; i < text.length(); i++){
        if(!text[i].isLower() && !text[i].isUpper()) {
            return false;
        }
    }
    return true;
}

void MainWindow::getWordsSlot(WordsType status, QString words)
{
    switch (status) {
    case IsWord:
        netWorkAccess_->accessWord(words);
        break;
    case IsSentence:
    {
        QStringList list = words.split("\n");
        if(list.size() <= 1){
            netWorkAccess_->accessSentence(words);
        }else{
            for(int i = 0; i < list.size(); ++i){
                if(isWord(list.at(i))) {
                    netWorkAccess_->accessWord(list.at(i));
                }else{
                    netWorkAccess_->accessSentence(list.at(i));
                }
            }
        }
    }
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

void MainWindow::updateAddListWgt()
{
    ui->addList->clear();
//    wordSentList_
    for(int i = 0; i < wordSentList_.size(); ++i){
        addWordListSlot(wordSentList_.at(i));
    }
}

void MainWindow::delWordListDataByName(QString name){
    for(int i = 0; i < wordSentList_.size(); ++i){
        if(name == wordSentList_.at(i).m_WordSent) {
            wordSentList_.remove(i);
            break;
        }
    }
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

void MainWindow::setWindowStyle()
{
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
            ui->textEdit->clear();
            if(wordInfo.m_isWord) {
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

// 1 InitRepo
void MainWindow::initRepoAndFilePathCombox()
{
    disconnect(ui->repoPathComBox,&QComboBox::currentTextChanged,this,&MainWindow::setRepoPathSlot);
    for(int i = 0; i < userInfo_.m_RepoPathList.size(); ++i) {
        ui->repoPathComBox->addItem(userInfo_.m_RepoPathList.at(i).m_Name);
    }
    connect(ui->repoPathComBox,&QComboBox::currentTextChanged,this,&MainWindow::setRepoPathSlot);

    repoPathName_ = ui->repoPathComBox->currentText();
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
        disconnect(ui->subPathComBox,&QComboBox::currentTextChanged,this,&MainWindow::setSubPathSlot);
        ui->subPathComBox->addItems(operateFile_->getSubDirNames(repoPath));
        connect(ui->subPathComBox,&QComboBox::currentTextChanged,this,&MainWindow::setSubPathSlot);
        if(ui->subPathComBox->count()!=0) {
            subDirName_ = ui->subPathComBox->currentText();
            whoIsBoxSelection(BoxSelect::SubCombox);
            setSubPathSlot(ui->subPathComBox->currentText());
        }else{
            ui->numSpinBox->setValue(-1);
            whoIsBoxSelection(BoxSelect::RepoCombox);
            appendTextToLog("Repo's subDir is empty!");
        }
    }else{
        appendTextToLog("Repo Path error!");
        whoIsBoxSelection(BoxSelect::RepoCombox);
    }
}

void MainWindow::setRepoPathSlot(QString currentStr){
    repoPathName_ = currentStr;
    updateSubDirCombox();
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
        currentWordCount_ = operateFile_->getCurrentFileWordNum(repoPath + "/" + subDirName_ + "/" + currentFile_);
        disconnect(ui->numSpinBox,QOverload<int>::of(&QSpinBox::valueChanged),this,&MainWindow::numSpinBoxValueChangedSlot);
        ui->numSpinBox->setValue(num);
        connect(ui->numSpinBox,QOverload<int>::of(&QSpinBox::valueChanged),this,&MainWindow::numSpinBoxValueChangedSlot);
        emit ui->numSpinBox->valueChanged(num);
        setStatusBar("", true);
    }
}

void MainWindow::whoIsBoxSelection(BoxSelect select)
{
    boxSelect_ = select;
    switch (select) {
    case BoxSelect::RepoCombox:
        changRepoComStyle(true);
        changSubPathStyle(false);
        changNumStyle(false);
        currentWordCount_ = -1;
        break;

    case BoxSelect::SubCombox:
        changSubPathStyle(true);
        changNumStyle(false);
        changRepoComStyle(false);
        currentWordCount_ = -1;
        break;
    case BoxSelect::NumSpinBox:
        changSubPathStyle(false);
        changNumStyle(true);
        changRepoComStyle(false);
        break;
    default:  // None
        changSubPathStyle(false);
        changNumStyle(false);
        changRepoComStyle(false);
        currentWordCount_ = -1;
        break;
    }
}
void MainWindow::changNumStyle(bool flags){
    if(flags){
        ui->numSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}\nQSpinBox{\nborder: 1px solid #d9d5d2\n}\n");
    }else{
        ui->numSpinBox->setStyleSheet("QSpinBox::up-button,QSpinBox::down-button\n{\n	width:0px;\n}");
    }
}

void MainWindow::changSubPathStyle(bool flags){
    if(flags){
        ui->subPathComBox->setStyleSheet("border: 1px solid #d9d5d2");
    }else{
        ui->subPathComBox->setStyleSheet("");
    }
}

void MainWindow::changRepoComStyle(bool flags){
    if(flags){
        ui->repoPathComBox->setStyleSheet("border: 1px solid #d9d5d2");
    }else{
        ui->repoPathComBox->setStyleSheet("");
    }
}

void MainWindow::numSpinBoxValueChangedSlot(int num)
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
            currentWordCount_ = operateFile_->getCurrentFileWordNum(repoPath + "/" + subDirName_ + "/" + currentFile_);
        }else{
            appendTextToLog(u8"当前目录为：" + currentFile_);
            currentWordCount_ = -1;
        }
        whoIsBoxSelection(BoxSelect::NumSpinBox);
        setStatusBar("",true);
    }else{
        currentFile_.clear();
        whoIsBoxSelection(BoxSelect::SubCombox);
        setStatusBar("",false);

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
        pStatusLabelWordCount_->setText(QString::number( currentWordCount_) + "个");
    }else{
        QImage image(QString(":/qss/psblack/checkbox_checked_disable.png"));
        pStatusLabelIcon_->setPixmap(QPixmap::fromImage(image));
        pStatusLabelIcon_->setMinimumWidth(15);
        pStatusLabelMsg_->setText(u8"错误");
        pStatusLabelCurrentFile_->setText("|  " + currentFile_);
        pStatusLabelWordCount_->setText("");
    }
}

void MainWindow::initStatusBar(){
    pStatusLabelIcon_ = new QLabel("",this);
    pStatusLabelMsg_ = new QLabel("",this);
    pStatusLabelMsg_->setAlignment(Qt::AlignLeft);
    pStatusLabelCurrentFile_ = new QLabel("",this);
    pStatusLabelCurrentFile_->setAlignment(Qt::AlignHCenter);

    pStatusLabelWordCount_ = new QLabel("",this);
    pStatusLabelWordCount_->setAlignment(Qt::AlignHCenter);


    ui->statusBar->addWidget(pStatusLabelIcon_);
    ui->statusBar->addWidget(pStatusLabelMsg_);
    ui->statusBar->addWidget(pStatusLabelCurrentFile_);
    ui->statusBar->addPermanentWidget(pStatusLabelWordCount_);
}

void MainWindow::on_toolButton_clicked()
{
    openExPro_->OpenDirSlot(getRepoPathByName(repoPathName_));
}

void MainWindow::on_openFilePbn_clicked()
{
    QString repoPath = getRepoPathByName(repoPathName_);
    openExPro_->OpenMarkdownAndDirSlot(repoPath + "/" + subDirName_ + "/" + currentFile_);
}


void MainWindow::on_addWordSentPbn_clicked()
{
    if(wordSentList_.isEmpty()) {
        return;
    }
    QString repoPath = getRepoPathByName(repoPathName_);
    QString currentFile = repoPath + "/" + subDirName_ + "/" + currentFile_;
    for(int i = 0; i < ui->addList->count(); ++i) {
        for(int j = 0; j < wordSentList_.size(); ++j){
            if(ui->addList->item(i)->text() == wordSentList_.at(j).m_WordSent){
                WordSentInfo wordSent = wordSentList_.at(j);
                if(wordSent.m_isWord) {
                    if(operateFile_->appendWord(currentFile, wordSent)) {
                        appendTextToLog("添加 [" + wordSent.m_WordSent + "] 成功!");
                        currentWordCount_++;
                        pStatusLabelWordCount_->setText(QString::number( currentWordCount_) + "个");
                    }else{
                        appendTextToLog("添加 [" + wordSent.m_WordSent + "] 失败!");
                    }
                } else {
                    if(operateFile_->appendSentence(currentFile, wordSent)) {
                        currentWordCount_++;
                        pStatusLabelWordCount_->setText(QString::number( currentWordCount_) + "个");
                        appendTextToLog("添加 [" + wordSent.m_WordSent + "] 成功!");
                    }else{
                        appendTextToLog("添加 [" + wordSent.m_WordSent + "] 失败!");
                    }
                }
                break;
            }
        }
    }
    ui->addList->clear();
    wordSentList_.clear();
    ui->textEdit->clear();
}

void MainWindow::changeSelectSatusSlot() {
    QObject* senderObj = sender(); // 获取发信号的对象
    if (senderObj){
        QListWidget* listWgt =dynamic_cast<QListWidget *>(senderObj);
        listWgt->setSelectionMode(QAbstractItemView::MultiSelection);
    }
}

// 双击删除Item
void MainWindow::moveDelItemFromListSlot(const QModelIndex &index)
{
    QString itemName =  ui->addList->item(index.row())->text();
    delWordListDataByName(ui->addList->item(index.row())->text());
    // 2. del ui
    // 注意：删除了一个Item后，删除的Item后面所有Item的index都会发生变化。
    ui->addList->takeItem(index.row());
    ui->textEdit->clear();
    appendTextToLog(u8"删除\""+ itemName + u8"\"完毕 !");
}

void MainWindow::delItemFromAddListSlot() {
    bool selectDel = false;
    for (int i = 0; i < ui->addList->count(); ++i) {
        if(ui->addList->item(i)->isSelected()){
            QString oldName = ui->addList->item(i)->text();
            delWordListDataByName(oldName);
            // 2. del ui
            // 注意：删除了一个Item后，删除的Item后面所有Item的index都会发生变化。
            ui->addList->takeItem(i--);
            // 3. del data
            selectDel = true;
        }
    }
    if(selectDel){
        ui->textEdit->clear();
    }
}

void MainWindow::on_createMarkdownPbn_clicked()
{
    QString currentFileName;
    QString repoPath = getRepoPathByName(repoPathName_);
    QString path = repoPath + "/" + subDirName_ ;
    if(operateFile_->createMarkdownFile(path, currentFileName)){
        setSubPathSlot(subDirName_);
        openExPro_->OpenMarkdownAndDirSlot(path + "/" + currentFile_);
    }
}

///// action /////
void MainWindow::modifyIniFileSlot(){
    openExPro_->OpenJsonAndIniSlot(iniInfo_.m_IniPath);
    start();
}

void MainWindow::modifyConfSlot()
{
    openExPro_->OpenJsonAndIniSlot(userInfo_.m_UserPath);
    start();
}

void MainWindow::clearTabWgtSlot()
{
    ui->logText->clear();
}

void MainWindow::setSampleView(){
    ui->tabWgt->hide();
    ui->pathWgt->hide();
    ui->listPbnWgt->hide();
    setSampleViewByScreenRes();
}

void MainWindow::setNormalView(){
    ui->tabWgt->show();
    ui->pathWgt->show();
    ui->listPbnWgt->show();
    setNormalViewByScreenRes();
}

void MainWindow::simpleViewSlot()
{
    simpleViewNum_++;
    if(simpleViewNum_ % 2 != 0){
        setSampleView();
        ui->actionSimpleView->setText(u8"正常窗口");
        ui->actionSimpleView->setShortcut(QKeySequence("Ctrl+="));
    }else{
        setNormalView();
        ui->actionSimpleView->setText(u8"极简窗口");
        ui->actionSimpleView->setShortcut(QKeySequence("Ctrl+-"));
    }
}

void MainWindow::setSampleViewByScreenRes(){
    if(getScrrenRes() == ScreenRes::High){
        this->setMinimumSize(QSize(440, 240));
        this->resize(QSize(880, 480));
    }else{
        this->setMinimumSize(QSize(650, 350));
        this->resize(QSize(650, 350));
    }
}

void MainWindow::syncAddTimelySlot() {
    on_addWordSentPbn_clicked();
}

void MainWindow::on_syncPbn_clicked()
{
    isSyncStart_ = (!isSyncStart_);
    if(isSyncStart_){
        timerSync_->start();
        appendTextToLog(QDateTime::currentDateTime().toString("[HH:mm:ss]") + u8" 启动自动同步 !");
        ui->syncPbn->setStyleSheet("border: 1px solid #d9d5d2");
    }else{
        timerSync_->stop();
        appendTextToLog(QDateTime::currentDateTime().toString("[HH:mm:ss]") + u8" 关闭自动同步 !");
        ui->syncPbn->setStyleSheet("");
    }

}
