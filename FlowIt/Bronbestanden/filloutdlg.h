#ifndef FILLOUTDLG_H
#define FILLOUTDLG_H

#include <QDialog>
#include "flowit.h"

namespace Ui {
class FilloutDlg;
}

#define FILMODE_DEVICE  1
#define FILMODE_MODULE  2
#define FILMODE_GENERAL 3

class FilloutDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FilloutDlg(QWidget *parent = 0);
    ~FilloutDlg();

    void setMode( int mode, QString declaration = "", QString instance = "");
    void selectMode( int mode);

    void addRoutine( QString routine);

    void setChartText( QString text);
    void setCodeText( QString text);

    QString chartText();
    QString codeText();

private slots:
    void on_rbDevice_clicked();
    void on_cbDevice_currentIndexChanged(const QString &arg1);
    void on_cbFuncDev_currentTextChanged(const QString &arg1);
    void on_cbFuncDev_currentIndexChanged(const QString &arg1);
    void on_rbCode_clicked();
    void on_pbAccept_clicked();
    void on_pbReject_clicked();

private:
    Ui::FilloutDlg *ui;
    int     m_mode;
    QString m_func;
    int     m_funcix;
};

#endif // FILLOUTDLG_H
