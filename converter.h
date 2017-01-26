/*
 *  chm2qch - Tool for converting Windows CHM files to Qt Help format.
 *  Copyright (C) 2016 Mitrich Software, bitbucket.org/mitrich_k/chm2qch
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONVERTER_H
#define CONVERTER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QProcess>

class ChmFile;

class Converter: public QObject
{
    Q_OBJECT

public:
    Converter(QObject *parent = 0);

    bool    guiMode;
    bool    quiet;
    bool    generate;
    bool    clean;
    bool    writeRoot;
    QString nameSpace;
    QString destDir;
    QString fileName;
    QString outName;
    QString qtDir;
    bool    canceled;

public slots:
    bool run();

signals:
    void statusChanged(const QString &status);
    void progressChanged(int p);
    void finished(bool ok, QString msg);

private:
    QString fileSystemName(const QString &objname);
    QString namespaceFromTitle(QString title, bool full = true);
    void    writeFile(const QString &filename, const QByteArray &data);
    void    writeQhp(const QString &filename, ChmFile *chm, const QString &nameSpace, bool writeRoot = true);
    bool    runQhg(const QString &qhpname);
    void    cleanFiles();
    void    msg(const QString &s0, const QString &s1 = QString());
    QProcess::ProcessError runProcess(const QString &program, const QStringList &args);

    QStringList generatedFiles;
};

#endif // CONVERTER_H
