#include "exportdlg.h"
#include "ui_exportdlg.h"
#include <QPrinterInfo>
#include <QStandardPaths>
#include <QMessageBox>
#include <QFileDialog>
#include "language.h"
#include "flowit.h"

static int g_select = EX_ARDUINO;

ExportDlg::ExportDlg( QWidget *parent) :
    QDialog( parent),
    ui(new Ui::ExportDlg)
{
    ui->setupUi(this);
    setWindowFlags( Qt::CustomizeWindowHint);

    ui->lblTitle->setText( T_EXPTITLE);
    ui->rbArduino->setText( T_EXPARDUINO);
    ui->rbRaspberry->setText( T_EXPRASPBERRY);
    ui->rbPdf->setText( T_EXPFLOWCHART);
    ui->lblFile->setText( T_EXPSAVEAS);
    ui->rbPrint->setText( T_EXPPRINTCHART);
    ui->lblPrint->setText( T_EXPPRINTER);
    ui->pbOk->setText( T_EXPDO);
    ui->pbCancel->setText( T_EXPCANCEL);

    ui->cbPrint->addItems( QPrinterInfo::availablePrinterNames());
    ui->rbArduino->setChecked( true);
    enableExport( true);

    switch ( g_select ) {
    case EX_ARDUINO:    ui->rbArduino->setChecked( true); break;
    case EX_RASPBERRY:  ui->rbRaspberry->setChecked( true); break;
    case EX_FLOWCHART:  ui->rbPdf->setChecked( true); break;
    case EX_PRINTCHART: ui->rbPrint->setChecked( true); break;
    }

    int ix;
    if ( g_exportPath.isEmpty() )
        g_exportPath = g_savePath;
    m_path = g_exportPath;
    if ( (ix = m_path.lastIndexOf( '.')) >= 0 )
        m_path = m_path.left( ix);
    if ( (ix = m_path.lastIndexOf( '/')) >= 0 )
        m_file = m_path.right( m_path.length() - ix - 1);

    ui->leFile->setText( makeFilePath());
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
    return g_select;
}

QString ExportDlg::makeFilePath()
{
    QDir dir;
    QString path = m_path;

    switch ( g_select ) {
        case EX_ARDUINO: path += "/" + m_file + ".ino"; break;
        case EX_RASPBERRY: path += "/" + m_file + ".cpp"; break;
        case EX_FLOWCHART: path += ".pdf"; break;
    }
    return path;
}

void ExportDlg::on_rbArduino_clicked()
{
    g_select = EX_ARDUINO;
    ui->leFile->setText( makeFilePath());
    enableExport( true);
}

void ExportDlg::on_rbRaspberry_clicked()
{
    g_select = EX_RASPBERRY;
    ui->leFile->setText( makeFilePath());
    enableExport( true);
}

void ExportDlg::on_rbPdf_clicked()
{
    g_select = EX_FLOWCHART;
    ui->leFile->setText( makeFilePath());
    enableExport( true);
}

void ExportDlg::on_pbFile_clicked()
{
    QString mydocs = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation);
    QString suffix, ftypes;
    if ( g_select == EX_ARDUINO ) {
        suffix = "ino";
        ftypes = V_INOFILES;
    }
    else
    if ( g_select == EX_RASPBERRY ) {
        suffix = "cpp";
        ftypes = V_CPPFILES;
    }
    else {
        suffix = "pdf";
        ftypes = V_PDFFILES;
    }
    QFileDialog fd( this, T_EXPORT, mydocs, ftypes);
    fd.setDefaultSuffix( suffix);
    fd.setFileMode( QFileDialog::Directory);
    fd.setOption( QFileDialog::DontUseNativeDialog);
    fd.setAcceptMode( QFileDialog::AcceptOpen);
    if ( fd.exec() == QDialog::Rejected )
        return;
    m_path = fd.selectedFiles().first();
    if ( m_path.right( m_file.length()) != m_file )
        m_path += "/" + m_file;
    ui->leFile->setText( makeFilePath());
}

void ExportDlg::on_rbPrint_clicked()
{
    enableExport( false);
    g_select = EX_PRINTCHART;
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
    if ( QFile::exists( ui->leFile->text()) ) {
        QMessageBox mb;
        mb.setStandardButtons( QMessageBox::Yes | QMessageBox::No);
        mb.setText( I_FILEEXISTS);
        if ( mb.exec() == QMessageBox::No ) {
            grabKeyboard();
            reject();
            return;
        }
    }

    int ix;
    g_exportPath = ui->leFile->text();
    if ( (g_select == EX_ARDUINO || g_select == EX_RASPBERRY) &&
         ((ix = g_exportPath.lastIndexOf( '/')) >= 0) )
        g_exportPath = g_exportPath.left( ix);
    grabKeyboard();
    accept();
}

void ExportDlg::on_pbCancel_clicked()
{
    reject();
}
