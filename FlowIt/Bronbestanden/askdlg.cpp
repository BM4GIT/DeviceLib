#include "language.h"
#include "askdlg.h"
#include "ui_askdlg.h"

AskDlg::AskDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AskDlg)
{
    ui->setupUi(this);
    ui->pbVal->setText( T_ASKACCEPT);
    ui->pbCancel->setText( T_ASKREJECT);

    setWindowFlags( Qt::CustomizeWindowHint);
    m_valid = false;
}

AskDlg::~AskDlg()
{
    delete ui;
}

void AskDlg::setLabel( QString label)
{
    ui->lblNum->setText( label);
}

void AskDlg::setValue( int value)
{
    ui->sbNum->setValue( value);
}

int AskDlg::value()
{
    if ( m_valid )
        return ui->sbNum->value();
    else
        return -1;
}

void AskDlg::on_pbVal_clicked()
{
    m_valid = true;
    accept();
}

void AskDlg::on_pbCancel_clicked()
{
    m_valid = false;
    reject();
}
