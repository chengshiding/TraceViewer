#include "decryptlog.h"
#include <QFile>
#include <QDebug>

DecryptLog::DecryptLog()
{
}

void* DecryptLog::memstr(char *buf, const char* cRes, unsigned int count )
{
    char* pbPos= buf;
    unsigned int iStrLen = strlen(cRes);
    while ((unsigned int)(pbPos-buf) <= (count - iStrLen))
    {
        if (memcmp(pbPos, cRes, iStrLen) == 0)
        {
            return pbPos;
        }
        pbPos++;
    }
    return NULL;
}
void* DecryptLog::memstrR(char *buf, const char* cRes, unsigned int count )
{

    unsigned int iStrLen = strlen(cRes);
    if (count<iStrLen)
    {
        return NULL;
    }
    char* pbPos= buf+count-iStrLen;
    while ((pbPos-buf)>=0)
    {
        if (memcmp(pbPos, cRes, iStrLen) == 0)
        {
            return pbPos;
        }
        pbPos--;
    }
    return NULL;
}
void DecryptLog::Open_Decrypt(QString fileName, QString& fileRes)
{
    QFile fileDes(fileName);

    char cShutter[8] = {0x0B, 0x0A, 0x09, 0x08, 0x07, 0x0D, 0x04, 0x05};

    if(!fileDes.open(QIODevice::ReadOnly))
    {
        qDebug() << "File1 Open Fail!";
        return;
    }

    unsigned int uiFileLength = fileDes.size();
    QByteArray pbBuffer(0, uiFileLength);
    pbBuffer = fileDes.readAll();
    //qDebug() << pbBuffer.toHex();

    char * pbBegin = NULL;
    char * pbEnd = NULL;
    pbBegin = pbEnd = pbBuffer.data();
    pbEnd = (char*)memstr(pbBegin, "-->", uiFileLength-(pbBegin-pbBuffer.data()));

    while (pbEnd)
    {
        pbBegin = (char*)memstrR(pbBegin, "<!--", pbEnd-pbBegin);
        if (pbBegin)
        {
            int i=0;
            pbBegin += strlen("<!--");
            if (pbBegin == pbEnd)
            {
                fileRes.append("\r\n");
            }
            while (pbBegin<=pbEnd)
            {
                if (pbBegin == pbEnd)
                {
                    //fileRes.WriteString("\n");
                    break;
                }
                pbBuffer[i] = (*pbBegin)^cShutter[i%8];
                //qDebug("pbBuffer[i]=%x", pbBuffer[i]);
                pbBegin++;
                i++;
            }
            if (i>0)
            {
                pbBuffer[i] = '\r';
                pbBuffer[++i] = '\n';
                pbBuffer[++i] = 0;
                fileRes.append(QString::fromLocal8Bit(pbBuffer));
            }
        }
        if ((unsigned int)(pbEnd+strlen("-->")-pbBuffer.data()) >= uiFileLength)
            break;
        pbEnd+=strlen("-->");
        pbBegin = pbEnd;
        pbEnd = (char*)memstr(pbBegin, "-->", uiFileLength-(pbBegin-pbBuffer.data()));
    }
    fileDes.close();
    return;
}

