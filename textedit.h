#ifndef TEXTEDIT_H
#define TEXTEDIT_H
#include <QWidget>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QString>
#include <QVBoxLayout>
#include <QScrollBar>
#include "configurefile.h"

class TextEdit : public QWidget
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);
    void readDescryptFile(QString& fileName);
    void findButton(const QRegExp);
    void go_to_line(int);
    void go_to_previous_row();
    void go_to_next_row();
    void select_cur_line();
    QTextEdit *textEdit;
public slots:

private:    
    QVBoxLayout *mainLayout;
    bool isFirstTime;
    QVector<int> highLightRows;
    int curHighlightRow;

};
#endif // TEXTEDIT_H
