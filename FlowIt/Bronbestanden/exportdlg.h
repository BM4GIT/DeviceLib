#ifndef EXPORTDLG_H
#define EXPORTDLG_H

#include <QDialog>
#include <QPrinter>

#define EX_PRINTCHART   1
#define EX_ARDUINO      2
#define EX_RASPBERRY    3
#define EX_FLOWCHART    4

namespace Ui {
class ExportDlg;
}

class ExportDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDlg( QWidget *parent = 0);
    ~ExportDlg();

    QString printerName();
    QString filePath();

    int exportType();

private slots:

    void on_rbArduino_clicked();
    void on_rbRaspberry_clicked();
    void on_rbImage_clicked();
    void on_pbFile_clicked();
    void on_rbPrint_clicked();
    void on_pbOk_clicked();
    void on_pbCancel_clicked();

private:
    void enableExport( bool enable);

    Ui::ExportDlg *ui;

    QString m_mydocs;
    int     m_select;
};

#endif // EXPORTDLG_H
