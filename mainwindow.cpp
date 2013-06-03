#include "mainwindow.h"

#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <QDebug>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{ 
    tracewidget = new Tracewidget(this);
    hBoxLayout_0 = new QHBoxLayout;
    hBoxLayout_1 = new QHBoxLayout;
    mainLayout = new QVBoxLayout;

    filterPatternLineEdit = new QLineEdit;
    filterPatternLineEdit->setText("");

    filterPatternLabel = new QLabel(tr("Filter pattern:"));
    filterPatternLabel->setBuddy(filterPatternLineEdit);

    filterSyntaxComboBox = new QComboBox;
    filterSyntaxComboBox->addItem(tr("Regular expression"), QRegExp::RegExp);
    filterSyntaxComboBox->addItem(tr("Wildcard"), QRegExp::Wildcard);
    filterSyntaxComboBox->addItem(tr("Fixed string"), QRegExp::FixedString);

    filterCaseSensitivityCheckBox = new QCheckBox(tr("Case sensitive filter"));
    filterCaseSensitivityCheckBox->setChecked(false);

    /*
    connect(filterPatternLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(textFilterChanged()));
    */
    connect(filterPatternLineEdit, SIGNAL(returnPressed()),
            this, SLOT(textFilterChanged()));
    connect(filterSyntaxComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(textFilterChanged()));
    connect(filterCaseSensitivityCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(textFilterChanged()));
    connect(this, SIGNAL(start_filter(QRegExp, bool)),
            tracewidget, SLOT(start_filter(QRegExp, bool)));

    connect(tracewidget, SIGNAL(NoticeParent(bool)), this, SLOT(update_actions(bool)));
    connect(tracewidget, SIGNAL(SwitchStartFromStopTimer(bool)), this, SLOT(SwitchStartFromStopTimer(bool)));


    hBoxLayout_0->addWidget(filterPatternLabel);
    hBoxLayout_0->addWidget(filterPatternLineEdit);
    hBoxLayout_0->addWidget(filterSyntaxComboBox);
    hBoxLayout_0->addWidget(filterCaseSensitivityCheckBox);

    movie = new QMovie(QCoreApplication::applicationDirPath()+ "/images/wait.gif");
    //movie->start();
    //movie->setSpeed(400);
    movieLabel = new QLabel(this);
    movieLabel->setMovie(movie);
    movieLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    movieLabel->setStyleSheet("background-color:white");
    movieLabel->setFrameShadow(QFrame::Sunken);
    movieLabel->setFrameShape(QFrame::Panel);
    movieLabel->setVisible(false);

    hBoxLayout_1->addWidget(tracewidget);
    hBoxLayout_1->addWidget(movieLabel);

    mainLayout->addLayout(hBoxLayout_0);
    mainLayout->addLayout(hBoxLayout_1);
    //mainLayout->addLayout(stackedLayout);

    widget = new QWidget();
    setCentralWidget(widget);
    centralWidget()->setLayout(mainLayout);

    connect(tracewidget, SIGNAL(SwitchNormalFromWait(bool)), this, SLOT(SwitchNormalFromWait(bool)), Qt::DirectConnection);

    readConfigFile();
    createMenus();
    //setStatusBar();
    setWindowTitle(tr("Trace Viewer"));
    setMinimumSize(700, 500);
}

void MainWindow::readConfigFile()
{
    QSettings * settings = new QSettings(QCoreApplication::applicationDirPath()+"/Config.ini", QSettings::IniFormat);
     settings->setIniCodec("UTF-8");

     OpenFileShotCut = settings->value("ShotCut/OpenFile").toInt();
     OpenDirShotCut= settings->value("ShotCut/OpenDir").toInt();
     CloseFileShotCut= settings->value("ShotCut/CloseFile").toInt();
     ExitShotCut=settings->value("ShotCut/Exit").toString();
     FilterShotCut=settings->value("ShotCut/Filter").toString();
     StartTraceShotCut=settings->value("ShotCut/StartTrace").toString();
     StopTraceShotCut=settings->value("ShotCut/StopTrace").toString();
     MarkShotCut=settings->value("ShotCut/Mark").toString();
     PreviousMarkShotCut=settings->value("ShotCut/PreviousMark").toString();
     NextMarkShotCut=settings->value("ShotCut/NextMark").toString();

     PreviousKeyShotCut=settings->value("ShotCut/PreviousKey").toString();
     NextKeyShotCut=settings->value("ShotCut/NextKey").toString();

     if(settings->status() != 0)
     {
         delete settings;
         settings = NULL;
         exit(0);
     }
     delete settings;
     settings = NULL;
}

void MainWindow::SwitchNormalFromWait(bool Normal)
{
    if(Normal)
    {
        movieLabel->setVisible(false);
        movie->stop();
        tracewidget->setVisible(true);
    }
    else
    {
        if(movie->state() == QMovie::NotRunning)
        {
            //qDebug() << "movie->start()";
            movie->start();
        }
        tracewidget->setVisible(false);
        movieLabel->setVisible(true);
        qApp->processEvents();
        //movieLabel->update();
    }
}

void MainWindow::update_actions(bool opened)
{
    if(opened)
    {
        //startTraceAct->setEnabled(true);
        markAct->setEnabled(true);
        previousAct->setEnabled(true);
        nextAct->setEnabled(true);
        filterDialogAct->setEnabled(true);
        closefileAct->setEnabled(true);
        //closedirAct->setEnabled(true);
        previousKeyAct->setEnabled(true);
        nextKeyAct->setEnabled(true);
    }
    else
    {
        startTraceAct->setEnabled(false);
        markAct->setEnabled(false);
        previousAct->setEnabled(false);
        nextAct->setEnabled(false);
        filterDialogAct->setEnabled(false);
        closefileAct->setEnabled(false);
        //closedirAct->setEnabled(false);
        previousKeyAct->setEnabled(false);
        nextKeyAct->setEnabled(false);
    }
}
void MainWindow::SwitchStartFromStopTimer(bool startTrue)
{
    qDebug() << "startTrue = " << startTrue;
    if(startTrue)
    {
        startTraceAct->setEnabled(true);
        stopTraceAct->setEnabled(false);
    }
    else
    {
        startTraceAct->setEnabled(false);
        stopTraceAct->setEnabled(true);
    }
}

void MainWindow::textFilterChanged()
{
    QRegExp::PatternSyntax syntax =
            QRegExp::PatternSyntax(filterSyntaxComboBox->itemData(
                    filterSyntaxComboBox->currentIndex()).toInt());
    Qt::CaseSensitivity caseSensitivity =
            filterCaseSensitivityCheckBox->isChecked() ? Qt::CaseSensitive
                                                       : Qt::CaseInsensitive;

    QRegExp regExp(filterPatternLineEdit->text(), caseSensitivity, syntax);
    if(tracewidget->cur_open_filecounts > 0)   //isOpen log flie true or false
    {
        if(filterPatternLineEdit->text().isEmpty())
            emit start_filter(regExp, false);
        else
            emit start_filter(regExp, true);
    }
}

void MainWindow::createMenus()
{    
    fileMenu = menuBar()->addMenu(tr("File"));
    filterMenu = menuBar()->addMenu(tr("Filter"));
    traceMenu = menuBar()->addMenu(tr("Trace"));
    bookMarkMenu = menuBar()->addMenu(tr("BookMark"));

    openUserLog = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/file.png"), tr("Open UserLog"), this);
    openMessageLog = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/file.png"), tr("Open MessageLog"), this);
    openComponentLog = new QAction(tr("Open ComponentLog"), this);
    openComponentMenu = new QMenu(this);
    //openComponentMenu = new QMenu(tr("Open ComponentLog"), this);

    openComponentLog->setMenu(openComponentMenu);

    fileMenu->addAction(openUserLog);
    connect(openUserLog, SIGNAL(triggered()), tracewidget, SLOT(open_user_file()));
    fileMenu->addAction(openMessageLog);
    connect(openMessageLog, SIGNAL(triggered()), tracewidget, SLOT(open_message_file()));
    fileMenu->addAction(openComponentLog);

    openfileAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/file.png"), tr("Open file"), this);
    openfileAct->setShortcut(QKeySequence(OpenFileShotCut));
    openfileAct->setStatusTip(tr("Open an existing component log file"));
    openComponentMenu->addAction(openfileAct);
    connect(openfileAct, SIGNAL(triggered()), tracewidget, SLOT(open_component_file()));

    opendirAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/dir.png"), tr("Open dir"), this);
    opendirAct->setShortcut(QKeySequence(OpenDirShotCut));
    openComponentMenu->addAction(opendirAct);
    connect(opendirAct, SIGNAL(triggered()), tracewidget, SLOT(open_dir()));

    fileMenu->addSeparator();

    closefileAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/close.png"), tr("Close file"), this);
    closefileAct->setShortcut(QKeySequence(CloseFileShotCut));
    fileMenu->addAction(closefileAct);
    connect(closefileAct, SIGNAL(triggered()), tracewidget, SLOT(closeCurrentWidget()));

    fileMenu->addSeparator();

    exitAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/exit.png"), tr("Exit"), this);
    exitAct->setShortcut(QKeySequence(ExitShotCut));
    fileMenu->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    startTraceAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/start.png"), tr("StartTrace"), this);
    startTraceAct->setShortcut(QKeySequence(StartTraceShotCut));
    traceMenu->addAction(startTraceAct);
    connect(startTraceAct, SIGNAL(triggered()), tracewidget, SLOT(startTrace()));

    stopTraceAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/stop.png"), tr("StopTrace"), this);
    stopTraceAct->setShortcut(QKeySequence(StopTraceShotCut));
    traceMenu->addAction(stopTraceAct);
    connect(stopTraceAct, SIGNAL(triggered()), tracewidget, SLOT(stopTrace()));


    markAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/mark.png"), tr("Mark"), this);
    markAct->setShortcut(QKeySequence(MarkShotCut));
    bookMarkMenu->addAction(markAct);
    connect(markAct, SIGNAL(triggered()), tracewidget, SLOT(mark()));

    previousAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/previous.png"), tr("PreviousMark"), this);
    previousAct->setShortcut(QKeySequence(PreviousMarkShotCut));
    bookMarkMenu->addAction(previousAct);
    connect(previousAct, SIGNAL(triggered()), tracewidget, SLOT(go_previous_mark()));

    nextAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/next.png"), tr("NextMark"), this);
    nextAct->setShortcut(QKeySequence(NextMarkShotCut));
    bookMarkMenu->addAction(nextAct);
    connect(nextAct, SIGNAL(triggered()), tracewidget, SLOT(go_next_mark()));

    menuBar()->addSeparator();

    findMenu = menuBar()->addMenu(tr("Find"));
    previousKeyAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/previous.png"), tr("PreviousKey"), this);
    previousKeyAct->setShortcut(QKeySequence(PreviousKeyShotCut));
    findMenu->addAction(previousKeyAct);
    connect(previousKeyAct, SIGNAL(triggered()), tracewidget, SLOT(go_to_previous_row()));

    nextKeyAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/next.png"), tr("NextKey"), this);
    nextKeyAct->setShortcut(QKeySequence(NextKeyShotCut));
    findMenu->addAction(nextKeyAct);
    connect(nextKeyAct, SIGNAL(triggered()), tracewidget, SLOT(go_to_next_row()));


    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("Help"));

    filterDialogAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/filter.png"), tr("Filter"), this);
    filterDialogAct->setShortcut(QKeySequence(FilterShotCut));
    filterMenu->addAction(filterDialogAct);
    connect(filterDialogAct, SIGNAL(triggered()), tracewidget, SLOT(filter_dialog()));

    aboutAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/about.png"), tr("About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    helpMenu->addAction(aboutAct);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(QIcon(QCoreApplication::applicationDirPath()+ "/images/aboutQt.png"), tr("About Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    helpMenu->addAction(aboutQtAct);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    //startTraceAct->setEnabled(true);
    startTraceAct->setEnabled(false);
    stopTraceAct->setEnabled(false);

    markAct->setEnabled(false);
    previousAct->setEnabled(false);
    nextAct->setEnabled(false);

    filterDialogAct->setEnabled(false);

    previousKeyAct->setEnabled(false);
    nextKeyAct->setEnabled(false);

    closefileAct->setEnabled(false);
}
void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> is a log file viewer"));
}
MainWindow::~MainWindow()
{
    
}
