#ifndef DECRYPTLOG_H
#define DECRYPTLOG_H
#include <QFile>
#include <QByteArray>
#include <QTextStream>
class DecryptLog
{
public:
    DecryptLog();
    void Open_Decrypt(QString, QString &);
    void* memstr(char *buf, const char* cRes, unsigned int count );
    void* memstrR(char *buf, const char* cRes, unsigned int count );

};

#endif // DECRYPTLOG_H
