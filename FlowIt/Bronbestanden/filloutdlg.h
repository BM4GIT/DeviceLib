#ifndef FILLOUTDLG_H
#define FILLOUTDLG_H

#include <QDialog>
#include "flowit.h"

namespace Ui {
class FilloutDlg;
}

class FilloutDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FilloutDlg(QWidget *parent = 0);
    ~FilloutDlg();

    void addRoutine( QString routine);
    void addInstance( QString instance);
    void selectInstance( bool select);

    void setChartText( QString text);
    void setCodeText( QString text);

    QString chartText();
    QString codeText();

private slots:
    void on_pbOK_clicked();
    void on_rbFunc_toggled(bool checked);
    void on_rbCode_toggled(bool checked);
    void on_cbInst_currentIndexChanged(const QString &arg1);
    void on_cbFunc_currentTextChanged(const QString &arg1);
    void on_cbFunc_currentIndexChanged(const QString &arg1);

private:
    Ui::FilloutDlg *ui;
    QString m_func;
    int     m_funcix;
};

#endif // FILLOUTDLG_H
