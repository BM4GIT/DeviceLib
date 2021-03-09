#include "language.h"
#include "filloutdlg.h"
#include "ui_filloutdlg.h"

FilloutDlg::FilloutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilloutDlg)
{
    ui->setupUi(this);
    ui->lblChart->setText( T_FILCHARTTEXT);
    ui->lblCode->setText( T_FILCODETEXT);
    ui->rbFunc->setText( T_FILFUNCCALL);
    ui->rbCode->setText( T_FILGENERAL);
    ui->lblFunc->setText( T_FILFUNCTION);
    ui->lblInst->setText( T_FILDECLARATION);

    setWindowFlags( Qt::CustomizeWindowHint);
    ui->cbCode->setCurrentIndex( -1);

    int type = -1;
    if ( g_foDrag ) type = g_foDrag->type();
    if ( g_owDrag ) type = g_owDrag->type();

    if ( type > -1 ) {
        for ( int i = 0; i < g_tools.size(); i++ ) {
            FlowObject* fo = g_tools.at( i)->root();
            if ( type == FOT_DO && fo->type() == FOT_ROUTINE &&
                 fo->name() != "setup" && fo->name() != "loop" )
                ui->cbCode->addItem( fo->name() + "()");
        }

        switch ( type ) {
            case FOT_CHECK:
            case FOT_DO:
            case FOT_WHILE:     for ( int i = 0; i < g_variables.size(); i++ ) {
                                    QString var = g_variables.at( i)->name;
                                    if ( type == FOT_DO )
                                        var += " = ";
                                    ui->cbCode->addItem( var);
                                }
        }
    }

    ui->cbCode->setCurrentIndex( -1);
}

FilloutDlg::~FilloutDlg()
{
    delete ui;
}

void FilloutDlg::addInstance( QString instance)
{
    if ( instance.isEmpty() )
        ui->rbCode->setChecked( true);
    else {
        ui->cbInst->addItem( instance);
        ui->cbInst->setCurrentIndex( -1); // force to change index and
        ui->cbInst->setCurrentIndex( 0);  // call on_cbInst_currentIndexChanged
    }
}

void FilloutDlg::addRoutine( QString routine)
{
    if ( ui->cbCode->findText( routine) < 0 )
        ui->cbCode->addItem( routine);
}

void FilloutDlg::setChartText( QString text)
{
    ui->leChart->setText( text);
}

void FilloutDlg::setCodeText( QString text)
{
    if ( ui->cbCode->findText( text) < 0 )
        ui->cbCode->addItem( text);
    ui->cbCode->setCurrentText( text);
    ui->rbCode->setChecked( true);
}

QString FilloutDlg::chartText()
{
    return ui->leChart->text();
}

QString FilloutDlg::codeText()
{
    if ( ui->rbFunc->isChecked() ) {
        QString txt = ui->cbFunc->currentText();
        if ( txt.left( 1) == "!" )
            return "!" + ui->cbInst->currentText() + "." + txt.right( txt.length() - 1);
        else
            return ui->cbInst->currentText() + "." + txt;
    }
    return ui->cbCode->currentText();
}

void FilloutDlg::on_pbOK_clicked()
{
    accept();
}

void FilloutDlg::selectInstance( bool select)
{
    ui->lblInst->setEnabled( select);
    ui->cbInst->setEnabled( select);
    ui->lblFunc->setEnabled( select);
    ui->cbFunc->setEnabled( select);
    ui->lblCode->setEnabled( !select);
    ui->cbCode->setEnabled( !select);
}

void FilloutDlg::on_rbFunc_toggled(bool checked)
{
    selectInstance( checked);
}

void FilloutDlg::on_rbCode_toggled(bool checked)
{
    selectInstance( !checked);
}

void FilloutDlg::on_cbInst_currentIndexChanged(const QString &arg1)
{
    if ( ui->cbInst->currentIndex() >= 0 ) {
        Template* templ = NULL;
        for ( int i = 0; i < g_objects.size(); i++ )
            if ( g_objects.at( i)->getInstance() == arg1 ) {
                templ = g_objects.at( i)->getTemplate();
                break;
            }
        ui->cbFunc->clear();
        if ( templ ) {
            for ( int i = 0; i < templ->functionCount(); i++ )
                ui->cbFunc->addItem( templ->getFunction( i));
            ui->rbFunc->setChecked( true);
        }
    }
}

void FilloutDlg::on_cbFunc_currentTextChanged(const QString &arg1)
{
    if ( m_func.isEmpty() ) {
        m_func = arg1.trimmed();
        if ( m_func.left( 1) == "!" )
            m_func = m_func.right( m_func.length() - 1);
        m_func = m_func.left( m_func.indexOf( "(") + 1);
    }
    else {
        QString arg = arg1.trimmed();
        if ( arg.indexOf( ")") < 0 ) {
            ui->cbFunc->setCurrentIndex( m_funcix);
            return;
        }
        if ( arg.left( 1) == "!" )
            arg = arg.right( arg.length() - 1);
        if ( arg.left( m_func.length()) != m_func )
            ui->cbFunc->setCurrentIndex( m_funcix);
    }
}

void FilloutDlg::on_cbFunc_currentIndexChanged(const QString &arg1)
{
    m_func = "";
    m_funcix = ui->cbFunc->currentIndex();
}
