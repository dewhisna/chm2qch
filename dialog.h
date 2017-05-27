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

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class QLineEdit;
class QAction;
class QThread;
class Converter;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(Converter *conv, QWidget *parent = 0);
    ~Dialog();

public slots:
    void reject();

protected:
    void showEvent(QShowEvent *e);

private slots:
    void selectFile();
    void selectDir();
    void selectQtDir();
    void enableOkBtn(const QString &text);
    void setProgressMode(bool v);
    void start();
    void stop();
    void finished(bool ok, QString msg);
    void showHelp();

signals:
    void runConverter();

private:
    void setupAutoComplete(QLineEdit *lineEdit, bool dirsOnly);
    void addEditAction(QLineEdit *lineEdit, QAction *action);
    void setupQtDirList();

    Ui::Dialog *ui;
    QAction *actSelectFile;
    QAction *actSelectDir;
    QAction *actSelectQtDir;
    Converter *converter;
    QThread *converterThread;
    bool converterRunning;
};

#endif // DIALOG_H
