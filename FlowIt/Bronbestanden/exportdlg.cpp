#include "exportdlg.h"
#include "ui_exportdlg.h"
#include <QPrinterInfo>
#include <QStandardPaths>
#include <QFileDialog>
#include <language.h>

ExportDlg::ExportDlg( QWidget *parent) :
    QDialog( parent),
    ui(new Ui::ExportDlg)
{
    ui->setupUi(this);
    setWindowFlags( Qt::CustomizeWindowHint);

    ui->lblTitle->setText( T_EXPTITLE);
    ui->rbArduino->setText( T_EXPARDUINO);
    ui->rbRaspberry->setText( T_EXPRASPBERRY);
    ui->rbImage->setText( T_EXPFLOWCHART);
    ui->lblFile->setText( T_EXPSAVEAS);
    ui->rbPrint->setText( T_EXPPRINTCHART);
    ui->lblPrint->setText( T_EXPPRINTER);
    ui->pbOk->setText( T_EXPDO);
    ui->pbCancel->setText( T_EXPCANCEL);

    ui->cbPrint->addItems( QPrinterInfo::availablePrinterNames());
    ui->rbArduino->setChecked( true);
    enableExport( true);
    m_mydocs = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation);
    QString app = "/" + V_OURAPP + ".ino";
    ui->leFile->setText( m_mydocs + app);
    m_select = EX_ARDUINO;
}

ExportDlg::~ExportDlg()
{
    delete ui;
}

QString ExportDlg::printerName()
{
    return ui->cbPrint->currentText();
}

QString ExportDlg::filePath()
{
    return ui->leFile->text();
}

int ExportDlg::exportType()
{
    return m_select;
}

void ExportDlg::on_rbArduino_clicked()
{
    QString app = "/" + V_OURAPP + ".ino";
    enableExport( true);
    m_select = EX_ARDUINO;
    ui->leFile->setText( m_mydocs + app);
}

void ExportDlg::on_rbRaspberry_clicked()
{
    QString app = "/" + V_OURAPP + ".cpp";
    enableExport( true);
    m_select = EX_RASPBERRY;
    ui->leFile->setText( m_mydocs + app);
}

void ExportDlg::on_rbImage_clicked()
{
    QString app = "/" + V_OURAPP + ".pdf";
    enableExport( true);
    m_select = EX_FLOWCHART;
    ui->leFile->setText( m_mydocs + app);
}

void ExportDlg::on_pbFile_clicked()
{
    QString mydocs = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation);
    QString suffix, ftypes;
    if ( m_select == EX_ARDUINO ) {
        suffix = "ino";
        ftypes = V_INOFILES;
    }
    else
    if ( m_select == EX_RASPBERRY ) {
        suffix = "cpp";
        ftypes = V_CPPFILES;
    }
    else {
        suffix = "png";
        ftypes = V_PNGFILES;
    }
    QFileDialog fd( this, T_EXPORT, mydocs, ftypes);
    fd.setDefaultSuffix( suffix);
    fd.setOption( QFileDialog::DontUseNativeDialog);
    fd.setAcceptMode( QFileDialog::AcceptOpen);
    if ( fd.exec() == QDialog::Rejected )
        return;
    ui->leFile->setText( fd.selectedFiles().first());
}

void ExportDlg::on_rbPrint_clicked()
{
    enableExport( false);
    m_select = EX_PRINTCHART;
}

void ExportDlg::enableExport( bool enable)
{
    ui->lblFile->setEnabled( enable);
    ui->leFile->setEnabled( enable);
    ui->pbFile->setEnabled( enable);
    ui->lblPrint->setEnabled( !enable);
    ui->cbPrint->setEnabled( !enable);
}

void ExportDlg::on_pbOk_clicked()
{
    accept();
}

void ExportDlg::on_pbCancel_clicked()
{
    reject();
}
