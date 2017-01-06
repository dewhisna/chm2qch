#ifndef CONVERTER_H
#define CONVERTER_H

#include <QString>
#include <QByteArray>
#include <QStringList>

class ChmFile;

class Converter
{
public:
    Converter();
    bool run();

    bool    quiet;
    bool    generate;
    bool    clean;
    bool    writeRoot;
    QString nameSpace;
    QString destDir;
    QString fileName;

private:
    QString fileSystemName(const QString &objname);
    QString namespaceFromTitle(QString title, bool full = true);
    void    writeFile(const QString &filename, const QByteArray &data);
    void    writeQhp(const QString &filename, ChmFile *chm, const QString &nameSpace, bool writeRoot = true);
    void    runQhg(const QString &qhpname);
    void    cleanFiles();
    void    msg(const QString &s0, const QString &s1 = QString());

    QStringList generatedFiles;
};

#endif // CONVERTER_H
