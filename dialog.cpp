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

#include "dialog.h"
#include "ui_dialog.h"
#include "converter.h"
#include "qtdirinfo.h"

#include <QCompleter>
#include <QFileSystemModel>
#include <QDir>
#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include <QFileInfo>
#include <QLineEdit>
#include <QSettings>
#include <QDebug>

#if QT_VERSION < 0x050200
#include <QToolButton>
#endif

Dialog::Dialog(Converter *conv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    converter = conv;

    ui->setupUi(this);
    setupAutoComplete(ui->fileName, false);
    setupAutoComplete(ui->destDir, true);
    setupAutoComplete(ui->qtDir->lineEdit(), true);
    actSelectFile = new QAction(QIcon(":/res/folder.png"), tr("Select input file"), this);
    actSelectDir  = new QAction(QIcon(":/res/folder.png"), tr("Select output directory"), this);
    actSelectQtDir= new QAction(QIcon(":/res/folder.png"), tr("Select Qt binaries directory"), this);
    addEditAction(ui->fileName, actSelectFile);
    addEditAction(ui->destDir,  actSelectDir);
    addEditAction(ui->qtDir->lineEdit(), actSelectQtDir);
    setupQtDirList();

    setProgressMode(false);
    ui->qtDirWidget->setVisible(false);

    ui->destDir->setText(converter->destDir);
    ui->nameSpace->setText(converter->nameSpace);
    ui->qtDir->setEditText(converter->qtDir);
    ui->writeRoot->setChecked(converter->writeRoot);
    ui->generate->setChecked(converter->generate);
    ui->clean->setChecked(converter->clean);

    connect(actSelectFile, SIGNAL(triggered()),          SLOT(selectFile()));
    connect(actSelectDir,  SIGNAL(triggered()),          SLOT(selectDir()));
    connect(actSelectQtDir,SIGNAL(triggered()),          SLOT(selectQtDir()));
    connect(ui->fileName,  SIGNAL(textChanged(QString)), SLOT(enableOkBtn(QString)));
    connect(ui->okBtn,     SIGNAL(clicked()),            SLOT(start()));
    connect(ui->cancelBtn, SIGNAL(clicked()),            SLOT(stop()));
    connect(ui->helpBtn,   SIGNAL(clicked()),            SLOT(showHelp()));
    connect(ui->toggleQtDir, SIGNAL(clicked(bool)), ui->qtDirWidget, SLOT(setVisible(bool)));

    converterThread = new QThread(this);
    converter->moveToThread(converterThread);

    connect(converter, SIGNAL(statusChanged(QString)), this,            SLOT(updateStatus(QString)));
    connect(converter, SIGNAL(progressChanged(int)),   ui->progressBar, SLOT(setValue(int)));
    connect(converter, SIGNAL(finished(bool,QString)), this,            SLOT(finished(bool,QString)));
    connect(this,      SIGNAL(runConverter()),         converter,       SLOT(run()));

    converterThread->start();
    converterRunning = false;
}

Dialog::~Dialog()
{
    delete ui;
    converterThread->quit();
    converterThread->wait();
}

void Dialog::reject()
{
    converter->saveSettings();
    QSettings s;
    s.setValue("window", saveGeometry());
    QDialog::reject();
}

void Dialog::showEvent(QShowEvent *e)
{
    QSettings s;
    restoreGeometry(s.value("window").toByteArray());
    QDialog::showEvent(e);
}

void Dialog::selectFile()
{
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setNameFilter("CHM help file (*.chm)");

    QDir dir(QFileInfo(ui->fileName->text()).path());

    if(dir.exists())
        dlg.setDirectory(dir.absolutePath());

    if(dlg.exec())
        ui->fileName->setText(dlg.selectedFiles().value(0));
}

void Dialog::selectDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select output directory"), ui->destDir->text());

    if(!dir.isEmpty())
        ui->destDir->setText(dir);
}

void Dialog::selectQtDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Qt binaries directory"), ui->qtDir->currentText());

    if(!dir.isEmpty())
        ui->qtDir->setEditText(dir);
}

void Dialog::enableOkBtn(const QString &text)
{
    ui->okBtn->setEnabled(!text.isEmpty());
}

void Dialog::setProgressMode(bool v)
{
    ui->fileName->setEnabled(!v);
    ui->destDir->setEnabled(!v);
    ui->nameSpace->setEnabled(!v);
    ui->writeRoot->setEnabled(!v);
    ui->generate->setEnabled(!v);
    ui->clean->setEnabled(!v);
    ui->okBtn->setEnabled(!v && !ui->fileName->text().isEmpty());

    ui->line->setVisible(v);
    ui->status->setVisible(v);
    ui->progressBar->setVisible(v);
    ui->progressBar->setRange(0, 100);
}

void Dialog::updateStatus(const QString &s)
{
    ui->status->setText(s);

    if(s.startsWith("Running qhelpgenerator"))
        ui->progressBar->setRange(0, 0);
}

void Dialog::start()
{
    QString fileName = ui->fileName->text();
    QString destDir = ui->destDir->text();

    if(destDir.isEmpty())
        destDir = QDir::currentPath();

    setProgressMode(true);

    converter->fileName  = fileName;
    converter->destDir   = destDir;
    converter->nameSpace = ui->nameSpace->text();
    converter->writeRoot = ui->writeRoot->isChecked();
    converter->generate  = ui->generate->isChecked();
    converter->clean     = ui->clean->isChecked();
    converter->qtDir     = ui->qtDir->currentText();
    converter->guiMode   = true;

    emit runConverter();
    converterRunning = true;
}

void Dialog::stop()
{
    if(!converterRunning)
        reject();

    converter->canceled = true;
}

void Dialog::finished(bool ok, QString msg)
{
    converterRunning = false;
    setProgressMode(false);

    if(ok)
        QMessageBox::information(this, "chm2qch", msg);
    else
        QMessageBox::warning(this, "chm2qch", msg);
}

void Dialog::showHelp()
{
    QFile f(":/res/help.html");
    f.open(QFile::ReadOnly);
    QString help = QString::fromUtf8(f.readAll());
    QMessageBox::information(this, "Help", help);
}

void Dialog::setupAutoComplete(QLineEdit *lineEdit, bool dirsOnly)
{
    QFileSystemModel *model = new QFileSystemModel(this);
    model->setRootPath(QDir::currentPath());
    model->setNameFilterDisables(false);

    if(dirsOnly)
        model->setFilter(QDir::Dirs);
    else
        model->setNameFilters({"*.chm"});

    QCompleter *completer = new QCompleter(this);
    completer->setModel(model);
    lineEdit->setCompleter(completer);
}

void Dialog::addEditAction(QLineEdit *lineEdit, QAction *action)
{
#if QT_VERSION >= 0x050200
    lineEdit->addAction(action, QLineEdit::TrailingPosition);
#else
    QToolButton *btn = new QToolButton(this);
    btn->setDefaultAction(action);
    btn->setAutoRaise(true);
    btn->setCursor(Qt::ArrowCursor);
    QHBoxLayout *l = new QHBoxLayout;
    l->setContentsMargins(0, 0, 0, 0);
    l->addStretch();
    l->addWidget(btn);
    lineEdit->setLayout(l);
#endif
}

void Dialog::setupQtDirList()
{
    QtVersionReader qv;

    if(!qv.read())
        return;

    QMapIterator<QString, QString> i(qv.versionInfo);

    while(i.hasNext())
    {
        i.next();

        if(i.key().startsWith("QtVersion"))
            ui->qtDir->addItem(QFileInfo(i.value()).path());
    }
}
