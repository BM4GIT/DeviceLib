#include "language.h"
#include "devicedlg.h"
#include "ui_devicedlg.h"
#include "objectwid.h"
#include <QMessageBox>

#define ADD_ROUTINE  0
#define ADD_VARIABLE 1
#define ADD_DEVICE   2
#define DEL_ROUTINE  3
#define DEL_VARIABLE 4
#define DEL_DEVICE   5

DeviceDlg::DeviceDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceDlg)
{
    ui->setupUi(this);
    ui->lblRoutine->setText( T_LBLROUTINE);
    ui->rbRoutine->setText( T_ADDROUTINE);
    ui->rbRoutineDel->setText( T_DELROUTINE);
    ui->lblVariable->setText( T_LBLVARIABLE);
    ui->rbVariable->setText( T_ADDVARIABLE);
    ui->rbVariableDel->setText( T_DELVARIABLE);
    ui->lblTypeVar->setText( T_TYPE);
    ui->lblDevice->setText( T_LBLDEVICE);
    ui->rbDevice->setText(( T_ADDDEVICE));
    ui->rbDeviceDel->setText(( T_DELDEVICE));
    ui->lblTypeDev->setText( T_TYPE);
    ui->lblClass->setText( T_DEVCLASS);
    ui->lblUsing->setText( T_DEVUSING);
    ui->lblInclude->setText( T_DEVINCLUDE);
    ui->lblDeclare->setText( T_DEVDECLARE);
    ui->pbDo->setText( T_DECLDO);
    ui->pbCancel->setText( T_DECLREJECT);

    ui->cbRoutine->lineEdit()->setReadOnly( true);
    ui->cbVariable->lineEdit()->setReadOnly( true);
    ui->cbDevice->lineEdit()->setReadOnly( true);
    ui->cbTypeDev->lineEdit()->setReadOnly( true);

    for ( int i = 2; i < g_tools.size(); i++ )
        if ( g_tools.at( i)->root()->type() == FOT_ROUTINE )
            ui->cbRoutine->addItem( g_tools.at( i)->root()->name());

    for ( int i = 0; i < g_variables.size(); i++ )
        ui->cbVariable->addItem( g_variables.at( i)->name);

    for ( int i = STANDARDOBJECTS; i < g_objects.size(); i++ )
        ui->cbDevice->addItem( g_objects.at( i)->name());

    ui->cbTypeVar->addItem( "int");
    ui->cbTypeVar->addItem( "long");
    ui->cbTypeVar->addItem( "float");
    ui->cbTypeVar->addItem( "double");
    ui->cbTypeVar->addItem( "char");
    ui->cbTypeVar->addItem( "String");
    ui->cbTypeVar->addItem( "StringList");
    ui->cbTypeVar->addItem( "bool");

    ui->cbTypeDev->addItem( T_ADDACTUATOR);
    ui->cbTypeDev->addItem( T_ADDSENSOR);
    ui->cbTypeDev->addItem( T_ADDSTORAGE);
    ui->cbTypeDev->addItem( T_ADDGUI);

    setWindowFlags( Qt::CustomizeWindowHint);
    m_fi = (FlowIt*) parent;
    ui->rbRoutine->setChecked( true);
    enable( ADD_ROUTINE);
}

DeviceDlg::~DeviceDlg()
{
    delete ui;
}

void DeviceDlg::setFI( FlowIt* fi)
{
    m_fi = fi;
}

void DeviceDlg::on_pbCancel_clicked()
{
    reject();
}

void DeviceDlg::on_pbDo_clicked()
{
    QMessageBox mb;

    // routine declaration
    if ( ui->rbRoutine->isChecked() ) {
        QString name = ui->leRoutine->text();
        // check the validity of the name
        if ( !g_validName( name) ) {
            mb.setStandardButtons( QMessageBox::Ok);
            mb.setText( I_INVALIDROUTINE);
            mb.exec();
            return;
        }
        // the specified name must be unique
        if ( m_fi->declarationExists( name) ) {
            mb.setStandardButtons( QMessageBox::Ok);
            mb.setText( I_DUPLICATENAME);
            mb.exec();
            return;
        }

        // create a tool
        Column* col = new Column;
        FlowRoutine* fr = new FlowRoutine( col);
        fr->init( name);
        m_fi->createTool( fr);
        accept();
        return;
    }

    // routine removal
    if ( ui->rbRoutineDel->isChecked() ) {
        QString name = ui->cbRoutine->currentText();
        m_fi->deleteTool( name);
        accept();
    }

    // variable declaration
    if ( ui->rbVariable->isChecked() ) {
        QString name = ui->leVariable->text();
        // check the validity of the name
        if ( !g_validName( name) ) {
            mb.setStandardButtons( QMessageBox::Ok);
            mb.setText( I_INVALIDROUTINE);
            mb.exec();
            return;
        }
        // the specified name must be unique
        if ( m_fi->declarationExists( name) ) {
            mb.setStandardButtons( QMessageBox::Ok);
            mb.setText( I_DUPLICATENAME);
            mb.exec();
            return;
        }
        m_fi->createVariable( ui->cbTypeVar->currentText(), name);
        accept();
        return;
    }

    // variable removal
    if ( ui->rbVariableDel->isChecked() ) {
        m_fi->deleteVariable( ui->cbVariable->currentText());
        accept();
        return;
    }

    // device declaration
    if ( ui->rbDevice->isChecked() ) {
        QString name = ui->leDevice->text();
        QString incl = ui->leInclude->text();
        QString vnam = ui->leDeclare->text();
        Template* templ = g_findTemplate( incl);

        // an include file must be specified
        if ( incl.isEmpty() ) {
            mb.setStandardButtons( QMessageBox::Ok);
            mb.setText( I_SUPPLYINCLUDE);
            mb.exec();
            return;
        }
        if ( templ->getModule().isEmpty() || !vnam.isEmpty() ) {
            // check the validity of the name
            if ( !g_validName( vnam) ) {
                mb.setStandardButtons( QMessageBox::Ok);
                mb.setText( I_INVALIDDECL);
                mb.exec();
                return;
            }
            // the specified name must be unique
            if ( m_fi->declarationExists( vnam) ) {
                mb.setStandardButtons( QMessageBox::Ok);
                mb.setText( I_DUPLICATENAME);
                mb.exec();
                return;
            }
        }

        // add the template to the used-library list
        m_fi->addLibrary( templ);

        // create the declaration object
        ObjectWid* ow;
        int fot = ui->cbTypeDev->currentIndex() + FOT_ACTUATOR;
        ow = new ObjectWid( m_fi, fot);
        m_fi->createObject( ow, name, templ->getModule().isEmpty());
        ow->setDeclaration( vnam, incl);
        m_fi->listObjects();

        accept();
    }

    // device removal
    if ( ui->rbDeviceDel->isChecked() ) {
        QString name = ui->cbDevice->currentText();
        m_fi->deleteObject( name);
        accept();
    }
}

void DeviceDlg::enable( int type)
{
    // routine
    ui->lblRoutine->setEnabled( type == ADD_ROUTINE || type == DEL_ROUTINE);
    ui->cbRoutine->setEnabled( type == ADD_ROUTINE || type == DEL_ROUTINE);
    ui->leRoutine->setEnabled( type == ADD_ROUTINE || type == DEL_ROUTINE);
    if ( type == ADD_ROUTINE ) { ui->cbRoutine->hide(); ui->leRoutine->show(); ui->leRoutine->setFocus(); }
    if ( type == DEL_ROUTINE ) { ui->cbRoutine->show(); ui->leRoutine->hide(); ui->cbRoutine->setFocus(); }

    // variable
    ui->lblVariable->setEnabled( type == ADD_VARIABLE || type == DEL_VARIABLE);
    ui->cbVariable->setEnabled( type == ADD_VARIABLE || type == DEL_VARIABLE);
    ui->leVariable->setEnabled( type == ADD_VARIABLE || type == DEL_VARIABLE);
    ui->lblTypeVar->setEnabled( type == ADD_VARIABLE);
    ui->cbTypeVar->setEnabled( type == ADD_VARIABLE);
    if ( type == ADD_VARIABLE ) { ui->cbVariable->hide(); ui->leVariable->show(); ui->leVariable->setFocus(); }
    if ( type == DEL_VARIABLE ) { ui->cbVariable->show(); ui->leVariable->hide(); ui->cbVariable->setFocus(); }

    // device
    ui->lblDevice->setEnabled( type == ADD_DEVICE || type == DEL_DEVICE);
    ui->cbDevice->setEnabled( type == ADD_DEVICE || type == DEL_DEVICE);
    ui->leDevice->setEnabled( type == ADD_DEVICE || type == DEL_DEVICE);
    ui->lblTypeDev->setEnabled( type == ADD_DEVICE);
    ui->cbTypeDev->setEnabled( type == ADD_DEVICE);
    ui->lblInclude->setEnabled( type == ADD_DEVICE);
    ui->leInclude->setEnabled( type == ADD_DEVICE);
    ui->lblUsing->setEnabled( type == ADD_DEVICE);
    ui->cbUsing->setEnabled( type == ADD_DEVICE);
    ui->lblClass->setEnabled( type == ADD_DEVICE);
    ui->cbClass->setEnabled( type == ADD_DEVICE);
    ui->lblDeclare->setEnabled( type == ADD_DEVICE);
    ui->leDeclare->setEnabled( type == ADD_DEVICE);
    if ( type == ADD_DEVICE ) { ui->cbDevice->hide(); ui->leDevice->show(); ui->leDevice->setFocus(); }
    if ( type == DEL_DEVICE ) { ui->cbDevice->show(); ui->leDevice->hide(); ui->cbDevice->setFocus(); }
}

void DeviceDlg::on_rbRoutine_clicked()
{
    enable( ADD_ROUTINE);
}

void DeviceDlg::on_rbRoutineDel_clicked()
{
    enable( DEL_ROUTINE);
}

void DeviceDlg::on_rbVariable_clicked()
{
    enable( ADD_VARIABLE);
}

void DeviceDlg::on_rbVariableDel_clicked()
{
    enable( DEL_VARIABLE);
}

void DeviceDlg::on_rbDevice_clicked()
{
    enable( ADD_DEVICE);
}

void DeviceDlg::on_rbDeviceDel_clicked()
{
    enable( DEL_DEVICE);
}

void DeviceDlg::on_cbTypeDev_currentIndexChanged(int index)
{
    ui->cbClass->clear();
    ui->cbUsing->clear();
    ui->leInclude->setText( "");
    ui->leDeclare->setText( "");

    int fot = index + FOT_ACTUATOR;
    for ( int i = 0; i < g_templates.size(); i++ ) {
        Template* templ = g_templates.at( i);
        if ( templ->fot() != fot )
            continue;
        QString part = templ->getClass();
        if ( part.isEmpty() )
            part = templ->getModule();
        if ( ui->cbClass->findText( part) == -1 )
            ui->cbClass->addItem( part);
    }
}

void DeviceDlg::on_cbClass_currentTextChanged(const QString &arg1)
{
    Template* templ = NULL;
    QString incl, use;
    ui->cbUsing->clear();
    ui->leInclude->setText( "");
    ui->leDeclare->setText( "");
    for ( int i = 0; i < g_templates.size(); i++ ) {
        templ = g_templates.at( i);
        if ( templ->getClass() == arg1 || templ->getModule() == arg1 ) {
            use = templ->getUsing();
            incl = templ->getInclude();
            if ( !use.isEmpty() && (ui->cbUsing->findText( use) == -1) )
                ui->cbUsing->addItem( use);
            ui->leDeclare->setEnabled( templ->getModule().isEmpty());
            break;
        }
    }
    if ( !ui->cbUsing->count() && !incl.isEmpty() ) {
        ui->cbUsing->setEnabled( false);
        ui->leInclude->setText( incl);
    }
    else
        ui->cbUsing->setEnabled( true);
}

void DeviceDlg::on_cbUsing_currentTextChanged(const QString &arg1)
{
    for ( int i = 0; i < g_templates.size(); i++ )
    {
        Template* templ = g_templates.at( i);
        if ( templ->getUsing() == arg1 ) {
            ui->leInclude->setText( templ->getInclude());
            break;
        }
    }
}
