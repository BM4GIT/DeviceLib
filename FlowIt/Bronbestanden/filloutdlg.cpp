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
    ui->rbCode->setText( T_FILGENERAL);
    ui->rbDevice->setText( T_FILDEVCALL);
    ui->lblDevice->setText( T_FILDEVICE);
    ui->lblFuncDev->setText( T_FILFUNCTION);

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
            case FOT_UNTIL:
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
    QString snot;

    text = text.trimmed();
    if ( text.left( 1) == "!" ) {
        text = text.right( text.length() - 1).trimmed();
        snot = "!";
    }

    if ( ui->cbCode->findText( text) < 0 ) // (>= 0) can occur with routines
        ui->cbCode->addItem( snot + text);
    ui->cbCode->setCurrentIndex( -1);
    ui->cbCode->setCurrentText( snot + text);

    selectMode( FILMODE_GENERAL);
}

QString FilloutDlg::chartText()
{
    return ui->leChart->text();
}

QString FilloutDlg::codeText()
{
    QString decl = ui->cbDevice->currentData().toString();
    if ( ui->rbDevice->isChecked() ) {
        if ( decl.isEmpty() )
            return ui->cbDevice->currentText() + "`" + ui->cbFuncDev->currentText();
        QString txt = ui->cbFuncDev->currentText();
        if ( txt.left( 1) == "!" )
            return "!" + decl + "." + txt.right( txt.length() - 1);
        else
            return decl + "." + txt;
    }
    return ui->cbCode->currentText();
}

void FilloutDlg::setMode( int mode, QString declaration, QString instance)
{
    m_mode = mode;
    if ( declaration.isEmpty() ) {
        for ( int i = STANDARDOBJECTS; i < g_objects.size(); i++ ) {
            ui->cbDevice->addItem( g_objects.at( i)->name());
            int ix = ui->cbDevice->count() - 1;
            QVariant var = QVariant( g_objects.at( i)->getInstance());
            ui->cbDevice->setItemData( ix, var);
        }
        selectMode( FILMODE_GENERAL);
    }
    else {
        ui->cbDevice->addItem( declaration);
        int ix = ui->cbDevice->count() - 1;
        QVariant var = QVariant( instance);
        ui->cbDevice->setItemData( ix, var);
        selectMode( FILMODE_DEVICE);
    }
    ui->cbDevice->setCurrentIndex( -1); // force to change index and
    ui->cbDevice->setCurrentIndex( 0);  // call on_cbModule_currentIndexChanged
}

void FilloutDlg::selectMode( int mode)
{
    ui->rbDevice->setChecked( mode == FILMODE_DEVICE);
    ui->lblDevice->setEnabled( mode == FILMODE_DEVICE);
    ui->cbDevice->setEnabled( mode == FILMODE_DEVICE);
    ui->lblFuncDev->setEnabled( mode == FILMODE_DEVICE);
    ui->cbFuncDev->setEnabled( mode == FILMODE_DEVICE);

    ui->rbCode->setChecked( mode == FILMODE_GENERAL);
    ui->lblCode->setEnabled( mode == FILMODE_GENERAL);
    ui->cbCode->setEnabled( mode == FILMODE_GENERAL);
}

void FilloutDlg::on_rbDevice_clicked()
{
    selectMode( FILMODE_DEVICE);
    m_func = "";
    m_funcix = ui->cbFuncDev->currentIndex();
}

void FilloutDlg::on_cbDevice_currentIndexChanged(const QString &arg1)
{
    if ( ui->cbDevice->currentIndex() >= 0 ) {
        Template* templ = NULL;
        for ( int i = 0; i < g_objects.size(); i++ )
            if ( g_objects.at( i)->name() == arg1 ) {
                templ = g_objects.at( i)->getTemplate();
                break;
            }
        ui->cbFuncDev->clear();
        if ( templ ) {
            for ( int i = 0; i < templ->functionCount(); i++ )
                ui->cbFuncDev->addItem( templ->getFunction( i));
        }
    }
}

void FilloutDlg::on_cbFuncDev_currentTextChanged(const QString &arg1)
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
            ui->cbFuncDev->setCurrentIndex( m_funcix);
            return;
        }
        if ( arg.left( 1) == "!" )
            arg = arg.right( arg.length() - 1);
        if ( arg.left( m_func.length()) != m_func )
            ui->cbFuncDev->setCurrentIndex( m_funcix);
    }
}

void FilloutDlg::on_cbFuncDev_currentIndexChanged(const QString &arg1)
{
    m_funcix = ui->cbFuncDev->currentIndex();
    m_func = "";
}

void FilloutDlg::on_rbCode_clicked()
{
    selectMode( FILMODE_GENERAL);
}

void FilloutDlg::on_pbAccept_clicked()
{
    accept();
}

void FilloutDlg::on_pbReject_clicked()
{
    reject();
}
