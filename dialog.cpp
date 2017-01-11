#include "dialog.h"
#include "ui_dialog.h"
#include "converter.h"

#include <QCompleter>
#include <QFileSystemModel>
#include <QDir>
#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include <QFileInfo>

#if QT_VERSION < 0x050200
#include <QToolButton>
#endif

Dialog::Dialog(Converter *conv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    converter = conv;

    ui->setupUi(this);
    setupAutoComplete(ui->fileName,  false);
    setupAutoComplete(ui->destDir, true);
    actSelectFile = new QAction(QIcon(":/images/folder.png"), tr("Select input file"), this);
    actSelectDir  = new QAction(QIcon(":/images/folder.png"), tr("Select output directory"), this);
    addEditAction(ui->fileName, actSelectFile);
    addEditAction(ui->destDir,  actSelectDir);
    setProgressMode(false);

    ui->destDir->setText(converter->destDir);
    ui->writeRoot->setChecked(converter->writeRoot);
    ui->generate->setChecked(converter->generate);
    ui->clean->setChecked(converter->clean);

    connect(actSelectFile, SIGNAL(triggered()),          SLOT(selectFile()));
    connect(actSelectDir,  SIGNAL(triggered()),          SLOT(selectDir()));
    connect(ui->fileName,  SIGNAL(textChanged(QString)), SLOT(enableOkBtn(QString)));
    connect(ui->okBtn,     SIGNAL(clicked()),            SLOT(start()));
    connect(ui->cancelBtn, SIGNAL(clicked()),            SLOT(stop()));

    converterThread = new QThread(this);
    converter->moveToThread(converterThread);

    connect(converter, SIGNAL(statusChanged(QString)), ui->status,      SLOT(setText(QString)));
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
    converter->writeRoot = ui->writeRoot->isChecked();
    converter->generate  = ui->generate->isChecked();
    converter->clean     = ui->clean->isChecked();
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
