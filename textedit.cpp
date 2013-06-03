#include "textedit.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <decryptlog.h>
#include <QDesktopWidget>
#include <QApplication>
TextEdit::TextEdit(QWidget *parent)
    : QWidget(parent)
{
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    this->setAutoFillBackground(true);
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(textEdit);
    this->setLayout(mainLayout);
    isFirstTime = true;
}
void TextEdit::readDescryptFile(QString& fileName)
{
    DecryptLog decrypt;
    QString text;
    decrypt.Open_Decrypt(fileName, text);
    textEdit->append(text);
}

void TextEdit::findButton(const QRegExp searchString)
{
    QTextDocument *document = textEdit->document();
    if (isFirstTime == false)
        document->undo();
    highLightRows.clear();
    curHighlightRow = -1;
    if (searchString.isEmpty())
    {
        return;
    }
    else
    {
        QTextCursor highlightCursor(document);
        QTextCursor cursor(document);
        cursor.beginEditBlock();
        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setBackground(Qt::yellow);

        //qDebug() << searchString.pattern().size();
        while (!highlightCursor.isNull() && !highlightCursor.atEnd())
        {
            //highlightCursor = document->find(searchString, highlightCursor, QTextDocument::FindWholeWords);
            highlightCursor = document->find(searchString, highlightCursor);

            if (!highlightCursor.isNull())
            {
                //highlightCursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
                //highlightCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, searchString.pattern().size());
                //highlightCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                highlightCursor.movePosition(QTextCursor::NoMove, QTextCursor::KeepAnchor);
                highlightCursor.mergeCharFormat(colorFormat);
                if(!highLightRows.contains(highlightCursor.blockNumber()))
                       highLightRows.append(highlightCursor.blockNumber());
            }
        }
        cursor.endEditBlock();
        isFirstTime = false;
        if(!highLightRows.isEmpty())
            curHighlightRow = highLightRows[0];
        select_cur_line();
    }
}


void TextEdit::go_to_previous_row()
{
    if(-1 == curHighlightRow)
        return;

    int goLineIndex = highLightRows.indexOf(curHighlightRow)-1;
    if(goLineIndex >= 0)
    {
        curHighlightRow = highLightRows[goLineIndex];
        go_to_line(curHighlightRow);
        /*
        QDesktopWidget* desktopWidget = QApplication::desktop();
        //得到客户区矩形
        QRect clientRect = desktopWidget->availableGeometry();
        //得到应用程序矩形
        //QRect applicationRect = desktopWidget->screenGeometry();
        int height = clientRect.height();
        //int height = applicationRect.height();
        qDebug() << height;
        textEdit->verticalScrollBar()->setValue(textEdit->verticalScrollBar()->value() - height/2);
        */
    }
}

void TextEdit::go_to_next_row()
{
    if(-1 == curHighlightRow)
        return;

    int goLineIndex = highLightRows.indexOf(curHighlightRow)+1;
    if(goLineIndex < highLightRows.size())
    {
        curHighlightRow = highLightRows[goLineIndex];
        go_to_line(curHighlightRow);
        /*
        QDesktopWidget* desktopWidget = QApplication::desktop();
        //得到客户区矩形
        QRect clientRect = desktopWidget->availableGeometry();
        //得到应用程序矩形
        //QRect applicationRect = desktopWidget->screenGeometry();
        int height = clientRect.height();
        //int height = applicationRect.height();
        qDebug() << height;
        textEdit->verticalScrollBar()->setValue(textEdit->verticalScrollBar()->value() + height/2);
        */
        //textEdit->verticalScrollBar()->setValue(textEdit->verticalScrollBar()->value() + 50);
    }
}
void TextEdit::go_to_line(int line)
{
    QTextDocument *document = textEdit->document();
    QTextCursor cursor(document);
    int position = document->findBlockByNumber (line).position();
    cursor.setPosition(position,QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::LineUnderCursor);
    textEdit->setTextCursor(cursor);
    textEdit->setFocus();
}
void TextEdit::select_cur_line()
{
    if(-1 != curHighlightRow)
    {
        int goLineIndex = highLightRows.indexOf(curHighlightRow);
        if(goLineIndex < highLightRows.size())
            go_to_line(curHighlightRow);
    }
}
