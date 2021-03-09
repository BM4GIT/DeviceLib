#ifndef ASKDLG_H
#define ASKDLG_H

#include <QDialog>

namespace Ui {
class AskDlg;
}

class AskDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AskDlg(QWidget *parent = 0);
    ~AskDlg();

    void setLabel( QString label);
    void setValue( int value);
    int value();

private slots:
    void on_pbVal_clicked();
    void on_pbCancel_clicked();

private:
    Ui::AskDlg *ui;
    bool m_valid;
};

#endif // ASKDLG_H
