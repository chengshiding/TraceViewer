#ifndef TRACEWIDGET_H
#define TRACEWIDGET_H

#include "configurefile.h"
#include "tablemodel.h"
#include "sortfilterproxymodel.h"
#include "textedit.h"
#include <QItemSelection>
#include <QTabWidget>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QTableView>
#include <QHeaderView>
#include <QVector>
#include <QTreeView>
#include <QTimer>
#include <QSettings>
#include <QClipboard>
#include <QApplication>
#include <QAction>
#include <QMenu>

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QItemSelectionModel;
QT_END_NAMESPACE

class Tracewidget : public QTabWidget
{
    Q_OBJECT
public:
    int cur_open_filecounts;
    void init();
    Tracewidget(QWidget *parent = 0);
    ~Tracewidget();
public slots:
    void open_dir();
    void cp_row();
    void detain_box(QModelIndex doubleClickedIndex);
    void show_contextmenu();
    void mouse_press(QPoint pos);
    void mouse_press_bad(QPoint pos);
    void mouse_press_old(QPoint pos);
    void filter_dialog();
    void start_filter(QRegExp regExp, bool filterEvent_bool);
    void startTrace();
    void stopTrace();
    void go_previous_mark();
    void go_next_mark();
    void mark();
    void checkFile();
    void closeTab(int);
    void closeCurrentWidget();
    void open_user_file();
    void open_message_file();
    void open_component_file();
    void go_to_previous_row();
    void go_to_next_row();

signals:
    void NoticeParent(bool);
    void SwitchStartFromStopTimer(bool);
    void SwitchNormalFromWait(bool);
    void activated(int);
private:
    void setupTabForDirfile(const QString&);
    void firstReadAllFiles();
    void getfilenames();
    void init_open_dir();
    void readFromFile(const QString &fileFullName, TableModel * table_of_file);
    void readFromFile_old(const QString &fileFullName, TableModel * table_of_file);
    int rtn_data_module_of_proxy(SortFilterProxyModel * cur_proxy);
    void readModifyFiles();
    void readModifyFile();
    bool ReadINI(QString &);
    void open_file();
    void readFromFile_C(QString& fileFullName, TableModel* table_of_file);
    void firstReadFile(QString &);
    void init_open_file();
    void setupTab(QString  &);
    QString getFileName(QString &);
    int rtn_point_of_textedit(TextEdit*);

    QString path;
    QStringList listFilenames;
    QVector<TableModel*> table;
    QVector<SortFilterProxyModel*> proxyModel;
    QTimer * timer;
    int alreadyRowsEmit;
    int CheckTimes;
    int fileColumns;
    int dirColumns;
    QStringList fileColumnNames;
    QStringList dirColumnNames;
    QSettings * settings;
    QString fileType;
    int READ_DECRYPT_LOG_USER;
    int READ_DECRYPT_LOG_MESSAGE;
    int READ_DECRYPT_LOG_COMPONENT;
    QMenu * cmenu;
    QAction * copy;
    QVector<TableModel*> file_table;
    QVector<SortFilterProxyModel*> file_proxyModel;
    QVector<QTableView*> file_displayView;

    QVector<TextEdit*> textEditVector;

#if DISPLAY_VIEW
    QVector<QTreeView*> displayView;
#else
    QVector<QTableView*> displayView;
#endif
};
#endif // TRACEWIDGET_H
