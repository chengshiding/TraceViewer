#include <QDebug>
#include <QFileDialog>
#include <QCoreApplication>
#include <QMessageBox>
#include "tracewidget.h"
#include "filterdialog.h"
#include "detaindialog.h"
#include <decryptlog.h>
#include "configurefile.h"

Tracewidget::Tracewidget(QWidget *parent)
    : QTabWidget(parent)
{
    timer = new QTimer(this);
    if(timer->isActive())
        timer->stop();
    connect(timer,SIGNAL(timeout()),this,SLOT(checkFile()));

    cur_open_filecounts = 0;

    setTabsClosable(true);
    setUsesScrollButtons(true);
    setMovable(true);

    cmenu = new QMenu(this);
    copy = cmenu->addAction(tr("copy"));
    connect(copy, SIGNAL(triggered(bool)), this, SLOT(cp_row()));
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}
Tracewidget::~Tracewidget()
{
    for (int i = 0; i < table.size(); ++i)
    {
        if(!table.isEmpty())
        {
            delete table[i];
            table[i] = NULL;
        }
        if(!proxyModel.isEmpty())
        {
            delete proxyModel[i];
            proxyModel[i] = NULL;
        }
        if(!displayView.isEmpty())
        {
            delete displayView[i];
            displayView[i] = NULL;
        }
    }
    for(int i=0; i<file_table.size(); ++i)
    {
        if(file_table[i]!=NULL)
        {
            delete file_table[i];
            file_table[i] = NULL;
        }
        if(file_proxyModel[i]!=NULL)
        {
            delete file_proxyModel[i];
            file_proxyModel[i] = NULL;
        }
        if(file_displayView[i]!=NULL)
        {
            delete file_displayView[i];
            file_displayView[i] = NULL;
        }
    }
    for(int i=0; i<textEditVector.size(); ++i)
    {
        if(textEditVector[i]!=NULL)
        {
            delete textEditVector[i];
            textEditVector[i] = NULL;
            textEditVector.remove(i);
        }
    }
}
void Tracewidget::init()
{
    QString sPath = QCoreApplication::applicationDirPath()+"/Config.ini";
    QFile File(sPath);
    if(!File.exists())
    {
        qDebug() << "Config.ini not exist";
        //qApp->exit(-1);
        exit(0);
    }
    qDebug() << "sPath = " << sPath;
    fileColumnNames.clear();
    dirColumnNames.clear();
    if(!ReadINI(sPath))
    {
        qDebug() << "Read Config.ini Error";
        exit(0);
    }
}

bool Tracewidget::ReadINI(QString &fileName)
{
    QSettings * settings = new QSettings(fileName, QSettings::IniFormat);
    settings->setIniCodec("UTF-8");
    CheckTimes = settings->value("Setup/CheckTimes", 1000).toInt();
    READ_DECRYPT_LOG_USER = settings->value("Setup/ReadDecryptUser", 0).toInt();
    READ_DECRYPT_LOG_MESSAGE = settings->value("Setup/ReadDecryptMessage", 0).toInt();
    READ_DECRYPT_LOG_COMPONENT = settings->value("Setup/ReadDecryptComponent", 0).toInt();
    fileColumns = settings->value("File/ColumnCount", 6).toInt();
    dirColumns = settings->value("Dir/ColumnCount", 4).toInt();

    fileColumnNames.append(settings->value("File/Column_1_Name").toString());
    fileColumnNames.append(settings->value("File/Column_2_Name").toString());
    fileColumnNames.append(settings->value("File/Column_3_Name").toString());
    fileColumnNames.append(settings->value("File/Column_4_Name").toString());
    fileColumnNames.append(settings->value("File/Column_5_Name").toString());
    fileColumnNames.append(settings->value("File/Column_6_Name").toString());

    dirColumnNames.append(settings->value("Dir/Column_1_Name").toString());
    dirColumnNames.append(settings->value("Dir/Column_2_Name").toString());
    dirColumnNames.append(settings->value("Dir/Column_3_Name").toString());
    dirColumnNames.append(settings->value("Dir/Column_4_Name").toString());
    if(settings->status() != 0)
    {
        delete settings;
        settings = NULL;
        return false;
    }
    delete settings;
    settings = NULL;
    return true;
}

void Tracewidget::open_file()
{
    init();
    QFileDialog *dlg = new QFileDialog;
    QString _fileFullPath = dlg->getOpenFileName();
    if (!_fileFullPath.isEmpty())
    {
        qDebug() << "fileFullPath = " << _fileFullPath;

        emit SwitchNormalFromWait(false);
        alreadyRowsEmit = 0;
        firstReadFile(_fileFullPath);
        setupTab(_fileFullPath);
        qDebug() << "file setup_done";
        emit SwitchNormalFromWait(true);
    }
    else
        qDebug() << "open file cancel";
}
void Tracewidget::open_user_file()
{
    fileType = "user_file";
    open_file();
}
void Tracewidget::open_message_file()
{
    init();
    textEditVector.append(new TextEdit);
    QFileDialog *dlg = new QFileDialog;
    QString _fileFullPath = dlg->getOpenFileName();
    if (_fileFullPath.isEmpty())
    {
        qDebug() << "fileFullPath is Empty";
        return;
    }

    DecryptLog decrypt;
    QString text;
    QTextStream in;
    int alreadyRowsEmit;
    QFile inputFile(_fileFullPath);
    QString lines;
    alreadyRowsEmit = 0;

    if(READ_DECRYPT_LOG_MESSAGE)
    {
        decrypt.Open_Decrypt(_fileFullPath, text);
        in.setString(&text);
    }
    else
    {
        inputFile.open(QIODevice::ReadOnly);
        in.setDevice(&inputFile);
    }
    while(!in.atEnd())
    {
        alreadyRowsEmit++;
        if(alreadyRowsEmit == ROWS_EMIT)
        {
            alreadyRowsEmit = 0;
            textEditVector.last()->textEdit->append(lines);
            lines.clear();
            emit SwitchNormalFromWait(false);
        }
        else
        {
            lines.append(in.readLine());
            lines.append("\n");
        }
    }
    if(!READ_DECRYPT_LOG_MESSAGE)
        inputFile.close();

    emit SwitchNormalFromWait(true);
    //fileType = "message_file";
    int index = addTab(textEditVector.last(), getFileName(_fileFullPath));
    setCurrentWidget(widget(index));
    currentWidget()->setObjectName("TextEdit");
    ++cur_open_filecounts;
    if(cur_open_filecounts > 0)
        NoticeParent(true);
    textEditVector.last()->setFocus();
    textEditVector.last()->textEdit->verticalScrollBar()->setValue(0);
}
void Tracewidget::open_component_file()
{
     fileType = "component_file";
     open_file();
}

QString Tracewidget::getFileName(QString  & fileFullPath)
{
    QString fileName;
    for(int i=fileFullPath.size()-1; i>=0; --i)
    {
        if(fileFullPath[i] != '/')
            fileName.insert(0,fileFullPath[i]);
        else
            break;
    }
    return fileName;
}

void Tracewidget::firstReadFile(QString & fileFullPath)
{
    if(fileType == "user_file")
    {
        file_table.append(new TableModel(fileColumns, fileColumnNames));
        file_table.last()->pos = 0;
        file_table.last()->cur_modify_time = QDateTime();
        readFromFile_C(fileFullPath, file_table.last());
        file_table.last()->headDataColor.resize(fileColumns);
        ++cur_open_filecounts;
        if(cur_open_filecounts > 0)
            NoticeParent(true);
    }
    else if(fileType == "message_file")
    {

    }
    else if(fileType == "component_file")
    {
        table.append(new TableModel(dirColumns, dirColumnNames));
        table.last()->pos = 0;
        table.last()->cur_modify_time = QDateTime();        
        readFromFile(fileFullPath, table.last());
        table.last()->headDataColor.resize(dirColumns);
        ++cur_open_filecounts;
        if(cur_open_filecounts > 0)
            NoticeParent(true);
    }
    else
    {
        qDebug() << "unexpected open file type";
        return;
    }
}
void Tracewidget::readModifyFile()
{

    //readFromFile_C(fileFullPath, file_table);
}

void Tracewidget::readFromFile_C(QString& fileFullName, TableModel* table_of_file)
{
    DecryptLog decrypt;
    QString text;
    QTextStream filein;

    QFileInfo fileInfo(fileFullName);
    QFile file(fileFullName);

    if(READ_DECRYPT_LOG_USER)
    {
        decrypt.Open_Decrypt(fileFullName, text);
        filein.setString(&text);
    }
    else
    {
        filein.setDevice(&file);
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug() << QString("open file \"[%1]\" error").arg(fileFullName);
            return;
        }
        if(table_of_file->cur_modify_time == fileInfo.lastModified())
        {
            qDebug() << "NOT MODIFY of file " << fileFullName;
            return;
        }
        filein.seek(table_of_file->pos);
    }
    QChar insertSpace[] = {QChar(' '),QChar(' '),QChar(' '),QChar(' '),QChar(' '),QChar(' '),QChar(' '),QChar(' '),QChar(' '),QChar(' ')};
    //QChar insertSpace[] = "          ";

    QString row_content;
    QStringList str_list;
    while(!filein.atEnd())
    {
        str_list.clear();
        row_content.clear();
        row_content = filein.readLine();
        //qDebug() << "row_content=" << row_content;
        alreadyRowsEmit++;
        if(alreadyRowsEmit == ROWS_EMIT)
        {
            alreadyRowsEmit = 0;
            emit SwitchNormalFromWait(false);
        }
        if(row_content.isEmpty())
            continue;

        if(row_content[9].unicode() >= 32768)
            row_content.insert(13, insertSpace, 4);
        str_list << row_content.mid(0,8);
        str_list << row_content.mid(9,14).trimmed();
        str_list << row_content.mid(26,12).trimmed();
        str_list << row_content.mid(39,12);
        str_list << row_content.mid(52,4);
        str_list << row_content.mid(59).trimmed();

        if(!table_of_file->cur_modify_time.isValid())
            table_of_file->listOfContent.append(str_list);

        if(!READ_DECRYPT_LOG_USER)
        {
            if(table_of_file->cur_modify_time != fileInfo.lastModified())
            {
                //qDebug() << "size(): " << table_of_file->listOfContent.size() << "str_list = " << str_list;
                table_of_file->cur_row_modify = str_list;
                table_of_file->insertRows(table_of_file->listOfContent.size(), 1, QModelIndex());
            }
        }
    }
    if(!READ_DECRYPT_LOG_USER)
    {
        table_of_file->pos = filein.pos();
        table_of_file->cur_modify_time = fileInfo.lastModified();

        if(file.isOpen())
            file.close();
    }
}
void Tracewidget::setupTab(QString & fileFullPath)
{
    if(fileType == "user_file")
    {
        file_proxyModel.append(new SortFilterProxyModel("SingleFile"));
        file_proxyModel.last()->setSourceModel(file_table.last());
        file_displayView.append(new QTableView);
        file_displayView.last()->setModel(file_proxyModel.last());
        file_displayView.last()->setSortingEnabled(false);
        file_displayView.last()->setSelectionBehavior(QAbstractItemView::SelectRows);
        file_displayView.last()->horizontalHeader()->setStretchLastSection(true);
#if HIDE_VERTICAL_HEAD_DATA
    file_displayView.last()->verticalHeader()->hide();
#endif
        file_displayView.last()->setEditTriggers(QAbstractItemView::NoEditTriggers);
        file_displayView.last()->setSelectionMode(QAbstractItemView::SingleSelection);
        file_displayView.last()->setShowGrid(false);
        file_displayView.last()->setWordWrap(false);
        file_displayView.last()->verticalHeader()->setDefaultSectionSize(17);
        file_displayView.last()->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(file_displayView.last(), SIGNAL(doubleClicked(QModelIndex)), this, SLOT(detain_box(QModelIndex)));
        connect(file_displayView.last(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(show_contextmenu()));
        int index = addTab(file_displayView.last(), getFileName(fileFullPath));
        setCurrentWidget(widget(index));
    }
    else if(fileType == "component_file")
    {
        proxyModel.append(new SortFilterProxyModel("DirFile"));
        proxyModel.last()->setSourceModel(table.last());
        displayView.append(new QTableView);
        displayView.last()->setModel(proxyModel.last());
        displayView.last()->setSortingEnabled(false);
        displayView.last()->setSelectionBehavior(QAbstractItemView::SelectRows);
        displayView.last()->horizontalHeader()->setStretchLastSection(true);
#if HIDE_VERTICAL_HEAD_DATA
    displayView.last()->verticalHeader()->hide();
#endif
        displayView.last()->setEditTriggers(QAbstractItemView::NoEditTriggers);
        displayView.last()->setSelectionMode(QAbstractItemView::SingleSelection);
        displayView.last()->setShowGrid(false);
        displayView.last()->setWordWrap(false);
        displayView.last()->verticalHeader()->setDefaultSectionSize(17);
        displayView.last()->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(displayView.last(), SIGNAL(doubleClicked(QModelIndex)), this, SLOT(detain_box(QModelIndex)));
        connect(displayView.last(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(show_contextmenu()));
        int index = addTab(displayView.last(), getFileName(fileFullPath));
        setCurrentWidget(widget(index));
    }
    else
    {
        qDebug() << "unexpected open file type";
        return;
    }
}

void Tracewidget::open_dir()
{

    init();
#if AUTO_OPEN_DIR
    QString pathName = "/mnt/soft/Yunio/work/log/20110309";
#else

    QString pathName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                         QCoreApplication::applicationDirPath(),
                                                         QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);

#endif

    if (!pathName.isEmpty())
    {
        qDebug() << pathName;
        emit SwitchNormalFromWait(false);
        alreadyRowsEmit = 0;
        path = pathName;
        listFilenames.clear();
        getfilenames();
        firstReadAllFiles();
        qDebug() << "setup_done";
        emit SwitchNormalFromWait(true);
    }
    else
        qDebug() << "open dir cancel";
}

void Tracewidget::checkFile()
{
    timer->stop();
    readModifyFiles();
    readModifyFile();
    timer->start(CheckTimes);
}

void Tracewidget::getfilenames()
{
    QStringList filters;
    QDir dir(path);
    if (!dir.exists())
        return;
    dir.setSorting(QDir::Name);
    filters << "*.log";
    dir.setNameFilters(filters);
    listFilenames = dir.entryList();
    qDebug() << "getfilenames() : listFilenames = " << listFilenames;
}
void Tracewidget::firstReadAllFiles()
{
    QString newFilepath;
    for (int i = 0; i < listFilenames.size(); ++i)
    {
        newFilepath = path + "/" + listFilenames.at(i);
        table.append(new TableModel(dirColumns, dirColumnNames));
        if(!READ_DECRYPT_LOG_COMPONENT)
        {
            table.last()->pos = 0;
            table.last()->cur_modify_time = QDateTime();
        }
        readFromFile(newFilepath, table.last());
        table.last()->headDataColor.resize(dirColumns);
        setupTabForDirfile(listFilenames.at(i));
        ++cur_open_filecounts;
        if(cur_open_filecounts > 0)
            NoticeParent(true);
    }
}
void Tracewidget::readModifyFiles()
{
    QString newFilepath;
    for (int i = 0; i < listFilenames.size(); ++i)
    {
        newFilepath = path + "/" + listFilenames.at(i);
        //qDebug() << "readModifyFiles() : newFilepath = " << newFilepath;
        readFromFile(newFilepath, table[i]);
    }
}
void Tracewidget::readFromFile(const QString &fileFullName, TableModel* table_of_file)
{
    DecryptLog decrypt;
    QString text;
    QTextStream filein;

    QFileInfo fileInfo(fileFullName);
    QFile file(fileFullName);
    if(READ_DECRYPT_LOG_COMPONENT)
    {
        decrypt.Open_Decrypt(fileFullName, text);
        filein.setString(&text);
    }
    else
    {
        filein.setDevice(&file);
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << QString("open file \"[%1]\" error").arg(fileFullName);
            return;
        }
        if(table_of_file->cur_modify_time == fileInfo.lastModified())
        {
            qDebug() << "NOT MODIFY of file " << fileFullName;
            return;
        }
        qDebug() << "readFromFile():";
        filein.seek(table_of_file->pos);
    }
    QString row_content;
    QStringList str_list;

    while(!filein.atEnd())
    {
        str_list.clear();
        row_content.clear();
        row_content = filein.readLine();
        alreadyRowsEmit++;
        if(alreadyRowsEmit == ROWS_EMIT)
        {
            alreadyRowsEmit = 0;
            emit SwitchNormalFromWait(false);
        }
        if(row_content.isEmpty())
            continue;
        //qDebug() << "row_content = " << row_content;

        str_list << row_content.mid(0,12);
        str_list << row_content.mid(13,3);
        str_list << row_content.mid(18,4).trimmed();
        str_list << row_content.mid(24).trimmed();

        if(!table_of_file->cur_modify_time.isValid())
            table_of_file->listOfContent.append(str_list);
        if(!READ_DECRYPT_LOG_COMPONENT)
        {
            if(table_of_file->cur_modify_time != fileInfo.lastModified())
            {
                //qDebug() << "size(): " << table_of_file->listOfContent.size() << "str_list = " << str_list;
                table_of_file->cur_row_modify = str_list;
                table_of_file->insertRows(table_of_file->listOfContent.size(), 1, QModelIndex());
            }
        }
    }
    qDebug() << "readfile";
    if(!READ_DECRYPT_LOG_COMPONENT)
    {
        table_of_file->pos = filein.pos();
        table_of_file->cur_modify_time = fileInfo.lastModified();
        if(file.isOpen())
            file.close();
    }
}


void Tracewidget::setupTabForDirfile(const QString& str)
{
    proxyModel.append(new SortFilterProxyModel("DirFile"));
    proxyModel.last()->setSourceModel(table.last());

#if DISPLAY_VIEW
    displayView.append(new QTreeView);
    displayView.last()->setRootIsDecorated(false);
    //displayView.last()->setAlternatingRowColors(true);
    displayView.last()->setModel(proxyModel.last());
    displayView.last()->setSortingEnabled(true);
    displayView.last()->setExpandsOnDoubleClick(false);
    displayView.last()->setSelectionBehavior(QAbstractItemView::SelectRows);
    displayView.last()->setEditTriggers(QAbstractItemView::NoEditTriggers);
    displayView.last()->setSelectionMode(QAbstractItemView::SingleSelection);

#else
    displayView.append(new QTableView);
    displayView.last()->setModel(proxyModel.last());
    //displayView.last()->setSortingEnabled(true); ////insertRow unnormally if true
    displayView.last()->setSortingEnabled(false);
    displayView.last()->setSelectionBehavior(QAbstractItemView::SelectRows);
    displayView.last()->horizontalHeader()->setStretchLastSection(true);
#if HIDE_VERTICAL_HEAD_DATA
    displayView.last()->verticalHeader()->hide();
#endif
    displayView.last()->setEditTriggers(QAbstractItemView::NoEditTriggers);
    displayView.last()->setSelectionMode(QAbstractItemView::SingleSelection);
    displayView.last()->setShowGrid(false);
    displayView.last()->setWordWrap(false);
    //设置QTableView的默认行高
    displayView.last()->verticalHeader()->setDefaultSectionSize(17);

#endif

    displayView.last()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(displayView.last(), SIGNAL(doubleClicked(QModelIndex)), this, SLOT(detain_box(QModelIndex)));
    connect(displayView.last(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(show_contextmenu()));
    int index = addTab(displayView.last(), str);
    setCurrentWidget(widget(index));
/*
    displayView.last()->horizontalHeader()->setAttribute(Qt::WA_OpaquePaintEvent);
    //displayView.last()->horizontalHeader()->setAttribute(Qt::WA_NoSystemBackground);
    displayView.last()->horizontalHeader()->setAutoFillBackground(true);

    QPalette p = displayView.last()->horizontalHeader()->palette();
    //p.setColor(QPalette::Window,Qt::green);
    p.setColor(QPalette::Base,Qt::red);
    p.setBrush(displayView.last()->horizontalHeader()->backgroundRole(), QBrush(Qt::red));
    displayView.last()->horizontalHeader()->setPalette(p);

    //displayView.last()->horizontalHeader()->setBackgroundRole(QPalette::Base);

    displayView.last()->horizontalHeader()->setModel(proxyModel.last());
    displayView.last()->horizontalHeader()->setStyle();
    displayView.last()->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
*/
}
void Tracewidget::closeCurrentWidget()
{
    closeTab(currentIndex());
}

void Tracewidget::closeTab(int index)
{
    if(index < 0)
        return;
    setCurrentWidget(widget(index));
    qDebug() << "Close Tab" << index;

    QObject *temp = static_cast<QObject*>(currentWidget());
    if(temp->objectName() == "TextEdit")
    {
        qDebug() << "message log close";
        TextEdit *tmp = static_cast<TextEdit*>(currentWidget());
        int i = rtn_point_of_textedit(tmp);
        if(i != -1)
        {
            delete textEditVector[i];
            textEditVector[i] = NULL;
            textEditVector.remove(i);

            --cur_open_filecounts;
            if(cur_open_filecounts <= 0)
                NoticeParent(false);
        }
        return;
    }
    QTableView *tmp = static_cast<QTableView*>(currentWidget());
    SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(tmp->model());

    int i_table = rtn_data_module_of_proxy(proxy);
    if(i_table == -1)
    {
        qDebug() << "closeTab(): unfound the datamodel";
        return;
    }
    if(proxy->type == "DirFile")
    {
        if(table.contains(table[i_table]))
        {
            delete table[i_table];
            table[i_table] = NULL;
            table.remove(i_table);

            delete proxyModel[i_table];
            proxyModel[i_table] = NULL;
            proxyModel.remove(i_table);

            delete displayView[i_table];
            displayView[i_table] = NULL;
            displayView.remove(i_table);

            --cur_open_filecounts;
            if(cur_open_filecounts <= 0)
                NoticeParent(false);
        }
    }
    else if(proxy->type == "SingleFile")
    {
        if(file_table.contains(file_table[i_table]))
        {
            delete file_table[i_table];
            file_table[i_table] = NULL;
            file_table.remove(i_table);

            delete file_proxyModel[i_table];
            file_proxyModel[i_table] = NULL;
            file_proxyModel.remove(i_table);

            delete file_displayView[i_table];
            file_displayView[i_table] = NULL;
            file_displayView.remove(i_table);

            --cur_open_filecounts;
            if(cur_open_filecounts <= 0)
                NoticeParent(false);
        }
    }
    else
    {
        qDebug() << "proxy->type Error";
        return;
    }
    //removeTab(index);
}


void Tracewidget::go_to_previous_row()
{
    QObject *temp = static_cast<QObject*>(currentWidget());
     if(temp->objectName() == "TextEdit")
     {
         TextEdit *textEdit = static_cast<TextEdit*>(currentWidget());
         int i = rtn_point_of_textedit(textEdit);
         if(i != -1)
             textEditVector[i]->go_to_previous_row();
     }
     else
     {
        QTableView *tmp = static_cast<QTableView*>(currentWidget());
        SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(tmp->model());
        QItemSelectionModel *selectionModel = tmp->selectionModel();
        QModelIndexList indexsSelected = selectionModel->selectedIndexes();
        if(indexsSelected.isEmpty())
            return;
        int start_row;
        QModelIndex index;
        if(proxy->curHighlightRow.row() == indexsSelected[0].row())
            start_row = -1;
        else
            start_row = indexsSelected[0].row();
        index = proxy->go_to_previous_row(start_row);
        if(index.isValid())
        {
            tmp->setCurrentIndex(proxy->index(index.row(), 0, QModelIndex()));
            //tmp->setCurrentIndex(index);
        }
     }
}

void Tracewidget::go_to_next_row()
{
    QObject *temp = static_cast<QObject*>(currentWidget());
     if(temp->objectName() == "TextEdit")
     {
         TextEdit *textEdit = static_cast<TextEdit*>(currentWidget());
         int i = rtn_point_of_textedit(textEdit);
         if(i != -1)
             textEditVector[i]->go_to_next_row();
     }
     else
     {
        QTableView *tmp = static_cast<QTableView*>(currentWidget());
        SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(tmp->model());
        QItemSelectionModel *selectionModel = tmp->selectionModel();
        QModelIndexList indexsSelected = selectionModel->selectedIndexes();
        if(indexsSelected.isEmpty())
            return;
        int start_row;
        QModelIndex index;
        if(proxy->curHighlightRow.row() == indexsSelected[0].row())
            start_row = -1;
        else
            start_row = indexsSelected[0].row();
        index = proxy->go_to_next_row(start_row);
        if(index.isValid())
        {
            tmp->setCurrentIndex(proxy->index(index.row(), 0, QModelIndex()));
            //tmp->setCurrentIndex(index);
        }
     }
}

void Tracewidget::start_filter(QRegExp regExp, bool filterEvent_bool)
{
    if(cur_open_filecounts <= 0)
        return;
    QObject *temp = static_cast<QObject*>(currentWidget());
    if(temp->objectName() == "TextEdit")
    {
        TextEdit *textEdit = static_cast<TextEdit*>(currentWidget());
        int i = rtn_point_of_textedit(textEdit);
        qDebug () << "start_filter(): message regExp=" << regExp;
        if(i != -1)
        {
            textEditVector[i]->findButton(regExp);
        }
        return;
    }
#if DISPLAY_VIEW
    QTreeView *tmp = static_cast<QTreeView*>(currentWidget());
#else
    QTableView *tmp = static_cast<QTableView*>(currentWidget());
#endif

   SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(tmp->model());
   int i_table = rtn_data_module_of_proxy(proxy);


#if NOT_HIDE_ROWS_WHEN_FILTER
   proxy->pHighlightIndexs.clear();
#endif

   if(-1 != i_table && proxy->type == "DirFile")
   {
       if(filterEvent_bool)
           table[i_table]->headDataColor[dirColumns-1] = true;
       else
           table[i_table]->headDataColor[dirColumns-1] = false;
       table[i_table]->headerDataChanged(Qt::Horizontal, dirColumns-1, dirColumns-1);
       proxy->setFilterRegExp(regExp);
   }
   else if(proxy->type == "SingleFile")
   {
       if(filterEvent_bool)
           file_table[i_table]->headDataColor[fileColumns-1] = true;
       else
           file_table[i_table]->headDataColor[fileColumns-1] = false;
       file_table[i_table]->headerDataChanged(Qt::Horizontal, fileColumns-1, fileColumns-1);
       proxy->setFilterRegExp(regExp);
   }   
   else
       qDebug() << "start_filter error";
}

void Tracewidget::filter_dialog()
{
    if(cur_open_filecounts <= 0)
        return;
    QObject *ttmp = static_cast<QObject*>(currentWidget());
    if(ttmp->objectName() == "TextEdit")
    {
        return;
    }

#if DISPLAY_VIEW
    QTreeView *temp = static_cast<QTreeView*>(currentWidget());
#else
    QTableView *temp = static_cast<QTableView*>(currentWidget());
#endif

    SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(temp->model());

    if(proxy->type == "DirFile")
    {
        QString MinFromString;
        QString MaxToString;
        QString ChoiceComponent;
        QString ChoiceOperation;
        FilterDialog filterDialog(proxy);
        if (filterDialog.exec())
        {
            MinFromString = filterDialog.fromDateEdit->text();
            MaxToString = filterDialog.toDateEdit->text();

#if TIME_FILTER_IS_STRING

#else
            //if(MinFromString == "0:00:00")
            MinFromString = "";
            //if(MaxToString == "0:00:00")
            MaxToString = "";
#endif

            proxy->setFilterMinimumDate(MinFromString);
            proxy->setFilterMaximumDate(MaxToString);

            ChoiceComponent = filterDialog.componentCombox->currentText();
            proxy->setfilterChoiceComponent(ChoiceComponent);
            ChoiceOperation = filterDialog.operationCombox->currentText();
            proxy->setFilterChoiceOperation(ChoiceOperation);
        }
        int i_table = rtn_data_module_of_proxy(proxy);
        if(MinFromString.isEmpty() && MaxToString.isEmpty())
        table[i_table]->headDataColor[0] = false;
        else
        {
            qDebug() << "NOT EMPTY" << MinFromString << MaxToString;
            table[i_table]->headDataColor[0] = true;
        }
        if(ChoiceComponent.isEmpty())
            table[i_table]->headDataColor[1] = false;
        else
            table[i_table]->headDataColor[1] = true;

        if(ChoiceOperation.isEmpty())
            table[i_table]->headDataColor[2] = false;
        else
            table[i_table]->headDataColor[2] = true;

        table[i_table]->headerDataChanged(Qt::Horizontal, 0, 2);
    }
    else if(proxy->type == "SingleFile")
    {
        QString MinFromString;
        QString MaxToString;
        QString ChoiceComponent;
        QString ChoiceOperation;
        FilterDialog filterDialog(proxy);
        if (filterDialog.exec())
        {
            MinFromString = filterDialog.fromDateEdit->text();
            MaxToString = filterDialog.toDateEdit->text();

#if TIME_FILTER_IS_STRING

#else
            //if(MinFromString == "0:00:00")
            MinFromString = "";
            //if(MaxToString == "0:00:00")
            MaxToString = "";
#endif

            proxy->setFilterMinimumDate(MinFromString);
            proxy->setFilterMaximumDate(MaxToString);

            ChoiceComponent = filterDialog.componentCombox->currentText();
            proxy->setfilterChoiceComponent(ChoiceComponent);
            ChoiceOperation = filterDialog.operationCombox->currentText();
            proxy->setFilterChoiceOperation(ChoiceOperation);
        }
        int i_table = rtn_data_module_of_proxy(proxy);
        if(-1 == i_table)
        {
            qDebug() << "unfound: i_table == -1";
            return;
        }

        if(MinFromString.isEmpty() && MaxToString.isEmpty())
            file_table[i_table]->headDataColor[3] = false;
        else
        {
            qDebug() << "NOT EMPTY" << MinFromString << MaxToString;
            file_table[i_table]->headDataColor[3] = true;
        }
        if(ChoiceComponent.isEmpty())
            file_table[i_table]->headDataColor[1] = false;
        else
            file_table[i_table]->headDataColor[1] = true;

        if(ChoiceOperation.isEmpty())
            file_table[i_table]->headDataColor[2] = false;
        else
            file_table[i_table]->headDataColor[2] = true;

        file_table[i_table]->headerDataChanged(Qt::Horizontal, 1, 3);
    }
    else
        qDebug() << "filter_dialog(): proxy->type ERROR";
}

void Tracewidget::go_previous_mark()
{
    if(cur_open_filecounts <= 0)
        return;
    QObject *ttmp = static_cast<QObject*>(currentWidget());
    if(ttmp->objectName() == "TextEdit")
    {
        return;
    }

#if DISPLAY_VIEW
    QTreeView *temp = static_cast<QTreeView*>(currentWidget());
#else
    QTableView *temp = static_cast<QTableView*>(currentWidget());
#endif

    SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(temp->model());

    QItemSelectionModel *selectionModel = temp->selectionModel();
    QModelIndexList indexsSelected = selectionModel->selectedIndexes();
    if(indexsSelected.isEmpty())
        return;
    int row = indexsSelected[0].row();
    if(proxy->curHighlightRow.row() == indexsSelected[0].row())
    {
        for(int i=proxy->markIndexs.indexOf(indexsSelected[0]) -1; i>=0; --i)
        {
            if(row > proxy->markIndexs[i].row())
            {
                proxy->curHighlightRow = proxy->markIndexs[i];
                temp->setCurrentIndex(proxy->markIndexs[i]);
                return;
            }
        }
    }
    else
    {
        for(int i=proxy->markIndexs.size()-1; i>=0; --i)
        {
            if(row > proxy->markIndexs[i].row())
            {
                proxy->curHighlightRow = proxy->markIndexs[i];
                temp->setCurrentIndex(proxy->markIndexs[i]);
                return;
            }
        }
    }

}

void Tracewidget::go_next_mark()
{
    if(cur_open_filecounts <= 0)
        return;
    QObject *ttmp = static_cast<QObject*>(currentWidget());
    if(ttmp->objectName() == "TextEdit")
    {
        return;
    }

#if DISPLAY_VIEW
    QTreeView *temp = static_cast<QTreeView*>(currentWidget());
#else
    QTableView *temp = static_cast<QTableView*>(currentWidget());
#endif

    SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(temp->model());

    QItemSelectionModel *selectionModel = temp->selectionModel();
    QModelIndexList indexsSelected = selectionModel->selectedIndexes();

    if(indexsSelected.isEmpty())
        return;
    int row = indexsSelected[0].row();
    if(proxy->curHighlightRow.row() == indexsSelected[0].row())
    {
        for(int i=proxy->markIndexs.indexOf(indexsSelected[0]) +1; i<proxy->markIndexs.size(); ++i)
        {
            if(row < proxy->markIndexs[i].row())
            {
                proxy->curHighlightRow = proxy->markIndexs[i];
                temp->setCurrentIndex(proxy->markIndexs[i]);
                return;
            }
        }
    }
    else
    {
        for(int i=0; i<proxy->markIndexs.size(); ++i)
        {
            if(row < proxy->markIndexs[i].row())
            {
                proxy->curHighlightRow = proxy->markIndexs[i];
                temp->setCurrentIndex(proxy->markIndexs[i]);
                return;
            }
        }
    }
}

void Tracewidget::mark()
{
    if(cur_open_filecounts <= 0)
        return;
    mouse_press(QPoint());
}
void Tracewidget::mouse_press_bad(QPoint pos)
{
    Q_UNUSED(pos);

    QObject *ttmp = static_cast<QObject*>(currentWidget());
    if(ttmp->objectName() == "TextEdit")
    {
        return;
    }

#if DISPLAY_VIEW
    QTreeView *temp = static_cast<QTreeView*>(currentWidget());
#else
    QTableView *temp = static_cast<QTableView*>(currentWidget());
#endif

    SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(temp->model());

    QItemSelectionModel *selectionModel = temp->selectionModel();
    QModelIndexList indexsSelected = selectionModel->selectedIndexes();

    if(indexsSelected.isEmpty())
        return;
    int row = indexsSelected[0].row();
    int j = 0;
    qDebug() << "Row = " << row;
    if(proxy->markRows.contains(row))
        proxy->markRows.removeAt(proxy->markRows.indexOf(row));
    else
    {
        if(proxy->markRows.isEmpty())
        {
            qDebug() << "append row";
            proxy->markRows.append(row);
        }
        else
        {
            for(j=0; j<proxy->markRows.size(); ++j)
            {
                if(row < proxy->markRows[j])
                {
                    proxy->markRows.insert(j, row);
                    break;
                }
            }
            if(j == proxy->markRows.size())
                proxy->markRows.append(row);
        }
     }
    proxy->headerDataChanged(Qt::Horizontal, 0, 3);
    //proxy->headerDataChanged(Qt::Vertical, row, 3);
}
void Tracewidget::mouse_press(QPoint pos)
{
    Q_UNUSED(pos);

    QObject *ttmp = static_cast<QObject*>(currentWidget());
    if(ttmp->objectName() == "TextEdit")
    {
        return;
    }

#if DISPLAY_VIEW
    QTreeView *temp = static_cast<QTreeView*>(currentWidget());
#else
    QTableView *temp = static_cast<QTableView*>(currentWidget());
#endif

    SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(temp->model());

    QItemSelectionModel *selectionModel = temp->selectionModel();
    QModelIndexList indexsSelected = selectionModel->selectedIndexes();

    if(indexsSelected.isEmpty())
        return;
    int row = indexsSelected[0].row();
    QModelIndex slectedIndex;
    int j = 0;

    slectedIndex = proxy->index(row, 0, QModelIndex());
    if(proxy->markIndexs.contains(slectedIndex))
        proxy->markIndexs.removeAt(proxy->markIndexs.indexOf(slectedIndex));
    else
    {
        if(proxy->markIndexs.isEmpty())
            proxy->markIndexs.append(slectedIndex);
        else
        {
            for(j=0; j<proxy->markIndexs.size(); ++j)
            {
                if(slectedIndex.row() < proxy->markIndexs[j].row())
                {
                    proxy->markIndexs.insert(j, slectedIndex);
                    break;
                }
            }
            if(j == proxy->markIndexs.size())
                proxy->markIndexs.append(slectedIndex);
        }
    }
    QVector<int> roles(0);
    roles.append(Qt::FontRole);
    roles.append(Qt::BackgroundRole);
    roles.append(Qt::ForegroundRole);
    proxy->dataChanged(proxy->index(row, 0, QModelIndex()), proxy->index(row, 0, QModelIndex()), roles);

#if MARK_GO_NEXT_ROW
    temp->setCurrentIndex(proxy->index(row+1, 0, QModelIndex()));
#endif

}
void Tracewidget::mouse_press_old(QPoint pos)
{
    Q_UNUSED(pos);

    QObject *ttmp = static_cast<QObject*>(currentWidget());
    if(ttmp->objectName() == "TextEdit")
    {
        return;
    }

#if DISPLAY_VIEW
    QTreeView *temp = static_cast<QTreeView*>(currentWidget());
#else
    QTableView *temp = static_cast<QTableView*>(currentWidget());
#endif

    SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(temp->model());

    QItemSelectionModel *selectionModel = temp->selectionModel();
    QModelIndexList indexsSelected = selectionModel->selectedIndexes();

    if(indexsSelected.isEmpty())
        return;
    int row = indexsSelected[0].row();
    QModelIndex slectedIndex;
    int j = 0;
    int Columns = 0;
    if(proxy->type == "DirFile")
        Columns = dirColumns;
    else if(proxy->type == "SingleFile")
        Columns = fileColumns;
    else
        qDebug() << "mouse_press() : proxy->type ERROR";

    for(int i=0; i<Columns; ++i)
    {
        slectedIndex = proxy->index(row, i, QModelIndex());
        if(proxy->markIndexs.contains(slectedIndex))
           proxy->markIndexs.removeAt(proxy->markIndexs.indexOf(slectedIndex));
        else
        {
            if(proxy->markIndexs.isEmpty())
            {
                proxy->markIndexs.append(slectedIndex);
            }
            else
            {
                for(j=0; j<proxy->markIndexs.size(); ++j)
                {
                    if(slectedIndex.row() < proxy->markIndexs[j].row())
                    {
                        proxy->markIndexs.insert(j, slectedIndex);
                        break;
                    }
                }
                if(j == proxy->markIndexs.size())
                    proxy->markIndexs.append(slectedIndex);
            }
        }
    }
    QVector<int> roles(0);
    roles.append(Qt::FontRole);
    roles.append(Qt::BackgroundRole);
    roles.append(Qt::ForegroundRole);
    proxy->dataChanged(proxy->index(row, 0, QModelIndex()), proxy->index(row, Columns-1, QModelIndex()), roles);
}

void Tracewidget::cp_row()
{

 #if DISPLAY_VIEW
     QTreeView *temp = static_cast<QTreeView*>(currentWidget());
 #else
     QTableView *temp = static_cast<QTableView*>(currentWidget());
 #endif

     SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(temp->model());

     QItemSelectionModel *selectionModel = temp->selectionModel();
     QModelIndexList indexsSelected = selectionModel->selectedIndexes();

     QClipboard* board = QApplication::clipboard();
     QString cpstr;
     QModelIndex index;
     QVariant var;
     cpstr.clear();
     if(indexsSelected.isEmpty())
         return;
     int row = indexsSelected[0].row();
     int Columns = 0;
     if(proxy->type == "DirFile")
         Columns = dirColumns;
     else if(proxy->type == "SingleFile")
         Columns = fileColumns;
     else
         qDebug() << "mouse_press() : proxy->type ERROR";

     for(int i=0; i<Columns; ++i)
     {
         index = proxy->index(row, i, QModelIndex());
         var = proxy->data(index, Qt::DisplayRole);
         cpstr += var.toString();
         cpstr += " ";
     }
     board->setText(cpstr);
}

void Tracewidget::detain_box(QModelIndex doubleClickedIndex)
{
#if DISPLAY_VIEW
    QTreeView *temp = static_cast<QTreeView*>(currentWidget());
#else
    QTableView *temp = static_cast<QTableView*>(currentWidget());
#endif

    SortFilterProxyModel *proxy = static_cast<SortFilterProxyModel*>(temp->model());
    int i_table = rtn_data_module_of_proxy(proxy);
    if(-1 != i_table && proxy->type == "DirFile")
    {
        DetainDialog detainDialog(doubleClickedIndex, proxy, temp, dirColumnNames);
        if (detainDialog.exec())
        {
        }
    }
   else if(-1 != i_table && proxy->type == "SingleFile")
   {
        DetainDialog detainDialog(doubleClickedIndex, proxy, temp, fileColumnNames);
        if (detainDialog.exec())
        {
        }
   }
   else
   {
       qDebug() << "detain_box() : unfound the datamodel";
       return;
   }
}


int Tracewidget::rtn_point_of_textedit(TextEdit* cur_widget)
{
    for(int i = 0; i < textEditVector.size(); ++i)
    {
        if(cur_widget == textEditVector[i])
            return i;
    }
    return -1;
}
int Tracewidget::rtn_data_module_of_proxy(SortFilterProxyModel * cur_proxy)
{
    int i;
    for(i = 0; i < proxyModel.size(); ++i)
    {
        if(cur_proxy == proxyModel[i])
            return i;
    }
    if(i == proxyModel.size())
    {
        for(i = 0; i < file_proxyModel.size(); ++i)
        {
            if(cur_proxy == file_proxyModel[i])
                return i;
        }
    }
    return -1;
}

void Tracewidget::startTrace()
{
    if(cur_open_filecounts > 0)
    {
        if(timer->isActive())
            timer->stop();
        return;
    }
    if(!timer->isActive())
        timer->start(CheckTimes);

    emit SwitchStartFromStopTimer(false);
}

void Tracewidget::stopTrace()
{
    if(timer->isActive())
        timer->stop();

    if(cur_open_filecounts <= 0)
        return;
    emit SwitchStartFromStopTimer(true);
}
void Tracewidget::show_contextmenu()
{
    cmenu->exec(QCursor::pos());//在当前鼠标位置显示
    //cmenu->exec(pos)是在viewport显示
}
