#include "chmfile.h"
#include "hhparser.h"

#include <chm_lib.h>
#include <QTextStream>
#include <QBuffer>
#include <QDebug>

ChmFile::ChmFile()
{
    defaultIndent = 3;
    handle = NULL;
}

ChmFile::ChmFile(const QString &filename)
{
    defaultIndent = 3;
    open(filename);
}

ChmFile::~ChmFile()
{
    close();
}

bool ChmFile::open(const QString &filename)
{
    handle = chm_open(filename.toLocal8Bit().data());

    if(!handle)
        return false;

    chm_enumerate(handle, CHM_ENUMERATE_NORMAL | CHM_ENUMERATE_FILES, &enumChmContents, (void *)this);
    readSystemData();

    return true;
}

void ChmFile::close()
{
    if(handle)
    {
        chm_close(handle);
        handle = NULL;
    }
}

QStringList ChmFile::objectList()
{
    return objNames;
}

QByteArray ChmFile::objectData(const QString &name)
{
    chmUnitInfo info;

    if(chm_resolve_object(handle, name.toLocal8Bit().data(), &info) != CHM_RESOLVE_SUCCESS)
    {
        qDebug() << "ChmFile::objectData: chm_resolve_object" << name << "failed";
        return QByteArray();
    }

    QByteArray arr(info.length, 0);
    chm_retrieve_object(handle, &info, (unsigned char *)arr.data(), 0, info.length);
    return arr;
}

QString ChmFile::title()
{
    return titleStr;
}

QString ChmFile::homeUrl()
{
    return defaultTopic;
}

QString ChmFile::tableOfContents(bool writeRoot)
{
    qDebug() << "Generating TOC";

    QString str;
    QTextStream stream(&str);

    QList<ParsedEntry> toc;
    HhParser p(this);
    p.parse(tocFileName(), toc, false);

    if(writeRoot)
    {
        defaultIndent = 4;
        stream << QString("<section title=\"%1\" ref=\"%2\">\n").arg(title()).arg(homeUrl());
    }

    for(int i = 0; i < toc.count(); i++)
    {
        bool hasChild = toc.value(i+1).indent > toc[i].indent;

        stream << indent(toc[i].indent);

        if(!hasChild)
            stream << QString("<section title=\"%1\" ref=\"%2\"/>\n").arg(toc[i].name, toc[i].urls.value(0).toString());
        else
            stream << QString("<section title=\"%1\" ref=\"%2\">\n").arg(toc[i].name, toc[i].urls.value(0).toString());

        int d = toc[i].indent - toc.value(i+1).indent;

        if(d)
        {
            for(int j = 0; j < d; j++)
                stream << indent(toc[i].indent - j - 1) << "</section>\n";
        }
    }

    if(writeRoot)
        stream << QString("</section>\n");

    return str;
}

QString ChmFile::index()
{
    qDebug() << "Generating Index";

    QString str;
    QTextStream stream(&str);

    QList<ParsedEntry> toc;
    HhParser p(this);
    p.parse(tocFileName(), toc, false);

    for(int i = 0; i < toc.count(); i++)
    {
        stream << indent(0) << QString("<keyword name=\"%1\" ref=\"%2\"/>\n").arg(toc[i].name, toc[i].urls.value(0).toString());
    }

    return str;
}

QString ChmFile::tocFileName() const
{
    foreach(const QString &str, objNames)
        if(str.endsWith(".hhc", Qt::CaseInsensitive))
            return str;

    return QString();
}

QString ChmFile::idxFileName() const
{
    foreach(const QString &str, objNames)
        if(str.endsWith(".hhk", Qt::CaseInsensitive))
            return str;

    return QString();
}

QString ChmFile::indent(int n)
{
    QString s;
    for(int i = 0; i < n + defaultIndent; i++)
        s.append("    ");
    return s;
}

void ChmFile::readSystemData()
{
    QByteArray array = objectData("/#SYSTEM");
    QBuffer buffer(&array);
    buffer.open(QIODevice::ReadOnly);

    unsigned int version;
    unsigned short code;
    unsigned short length;

    buffer.read((char *)&version, 4);

    while(!buffer.atEnd())
    {
        buffer.read((char *)&code, 2);
        buffer.read((char *)&length, 2);
        QByteArray data = buffer.read(length);

        if(code == 3)
            titleStr = QString(data);
        else if(code == 2)
            defaultTopic = QString(data);
    }
}

int enumChmContents(chmFile *h, chmUnitInfo *ui, void *context)
{
    Q_UNUSED(h);
    ChmFile *chm = (ChmFile *)context;
    chm->objNames.append(QString(ui->path));
    return CHM_ENUMERATOR_CONTINUE;
}

