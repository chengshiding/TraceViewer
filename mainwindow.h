#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <tracewidget.h>
#include <QWidget>
#include <QHBoxLayout>
#include <QtWidgets>
#include <QMovie>
#include "configurefile.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init_window();



signals:
    void start_filter(QRegExp, bool);
private slots:
    void textFilterChanged();
    void update_actions(bool);
    void SwitchNormalFromWait(bool);
    void SwitchStartFromStopTimer(bool);
    void about();

private:
    void createMenus();
    void readConfigFile();
    void setShotCut();
    Tracewidget *tracewidget;
    QMenu *fileMenu;
    QAction *openUserLog;
    QAction *openMessageLog;
    QAction *openComponentLog;
    QMenu *openComponentMenu;
    QAction *openfileAct;
    QAction *opendirAct;
    QAction *closefileAct;
    QAction *closedirAct;
    QAction *exitAct;

    QMenu *traceMenu;
    QAction *startTraceAct;
    QAction *stopTraceAct;

    QMenu *bookMarkMenu;
    QAction *markAct;
    QAction *previousAct;
    QAction *nextAct;

    QMenu *filterMenu;
    QAction *filterTimeAct;
    QAction *filterComponentAct;
    QAction *filterOperationAct;
    QAction *filterEventAct;
    QAction *filterDialogAct;

    QMenu *findMenu;
    QAction *previousKeyAct;
    QAction *nextKeyAct;

    QMenu *helpMenu;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QLabel *filterPatternLabel;
    QLineEdit *filterPatternLineEdit;
    QComboBox *filterSyntaxComboBox;
    QCheckBox *filterCaseSensitivityCheckBox;

    QLabel *movieLabel;
    QMovie *movie;
    QStackedLayout *stackedLayout;

    QHBoxLayout *hBoxLayout_0;
    QHBoxLayout *hBoxLayout_1;
    QVBoxLayout *mainLayout;

    QWidget *widget;

    QString OpenFileShotCut;
    QString OpenDirShotCut;
    QString CloseFileShotCut;
    QString ExitShotCut;
    QString FilterShotCut;
    QString StartTraceShotCut;
    QString StopTraceShotCut;
    QString MarkShotCut;
    QString PreviousMarkShotCut;
    QString NextMarkShotCut;
    QString PreviousKeyShotCut;
    QString NextKeyShotCut;
};

#endif // MAINWINDOW_H
