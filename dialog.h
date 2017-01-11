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

private slots:
    void selectFile();
    void selectDir();
    void enableOkBtn(const QString &text);
    void setProgressMode(bool v);
    void start();
    void stop();
    void finished(bool ok, QString msg);

signals:
    void runConverter();

private:
    void setupAutoComplete(QLineEdit *lineEdit, bool dirsOnly);
    void addEditAction(QLineEdit *lineEdit, QAction *action);

    Ui::Dialog *ui;
    QAction *actSelectFile;
    QAction *actSelectDir;
    Converter *converter;
    QThread *converterThread;
    bool converterRunning;
};

#endif // DIALOG_H
