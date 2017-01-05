#ifndef CHMFILE_H
#define CHMFILE_H

#include <QString>
#include <QStringList>
#include <QByteArray>

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
    QString tableOfContents(bool writeRoot = true);
    QString index();

private:
    QString tocFileName() const;
    QString idxFileName() const;
    QString indent(int n);
    void    readSystemData();

    chmFile *handle;
    QStringList objNames;
    QString titleStr;
    QString defaultTopic;
    int defaultIndent;

    friend int enumChmContents(chmFile *h, chmUnitInfo *ui, void *context);
};

int enumChmContents(chmFile *h, chmUnitInfo *ui, void *context);

#endif // CHMFILE_H
