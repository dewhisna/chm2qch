#ifndef CHMFILE_H
#define CHMFILE_H

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QXmlStreamWriter>

struct chmFile;
struct chmUnitInfo;

class ChmFile
{
public:
    ChmFile();
    ChmFile(const QString &filename);
    ~ChmFile();

    bool open(const QString &filename);
    void close();

    QStringList objectList();
    QByteArray objectData(const QString &name);

    QString title();
    QString homeUrl();

    void writeToc(QXmlStreamWriter &xml, bool writeRoot = true);
    void writeIndex(QXmlStreamWriter &xml);

private:
    QString tocFileName() const;
    QString idxFileName() const;
    void    readSystemData();

    chmFile *handle;
    QStringList objNames;
    QString titleStr;
    QString defaultTopic;

    friend int enumChmContents(chmFile *h, chmUnitInfo *ui, void *context);
};

int enumChmContents(chmFile *h, chmUnitInfo *ui, void *context);

#endif // CHMFILE_H
