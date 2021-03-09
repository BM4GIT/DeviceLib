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
    ui->lblDevice->setText( T_LBLDEVICE);
    ui->rbActuator->setText( T_ADDACTUATOR);
    ui->rbSensor->setText( T_ADDSENSOR);
    ui->rbDeviceDel->setText(( T_DELDEVICE));
    ui->lblType->setText( T_VARTYPE);
    ui->lblClass->setText( T_DEVCLASS);
    ui->lblUsing->setText( T_DEVUSING);
    ui->lblInclude->setText( T_DEVINCLUDE);
    ui->lblDeclare->setText( T_DEVDECLARE);
    ui->pbDo->setText( T_DECLDO);
    ui->pbCancel->setText( T_DECLREJECT);

    ui->cbRoutine->lineEdit()->setReadOnly( true);
    ui->cbVariable->lineEdit()->setReadOnly( true);
    ui->cbDevice->lineEdit()->setReadOnly( true);

    for ( int i = 2; i < g_tools.size(); i++ )
        if ( g_tools.at( i)->root()->type() == FOT_ROUTINE )
            ui->cbRoutine->addItem( g_tools.at( i)->root()->name());

    for ( int i = 0; i < g_variables.size(); i++ )
        ui->cbVariable->addItem( g_variables.at( i)->name);

    for ( int i = 6; i < g_objects.size(); i++ )
        ui->cbDevice->addItem( g_objects.at( i)->name());

    ui->cbType->addItem( "int");
    ui->cbType->addItem( "long");
    ui->cbType->addItem( "float");
    ui->cbType->addItem( "double");
    ui->cbType->addItem( "char");
    ui->cbType->addItem( "String");
    ui->cbType->addItem( "StringList");
    ui->cbType->addItem( "bool");

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
        m_fi->createVariable( ui->cbType->currentText(), name);
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
    if ( ui->rbActuator->isChecked() || ui->rbSensor->isChecked() ) {
        QString name = ui->leDevice->text();
        QString incl = ui->leInclude->text();
        QString clss = ui->cbClass->currentText();
        QString vnam = ui->leDeclare->text();

        // an include file must be specified
        if ( incl.isEmpty() ) {
            mb.setStandardButtons( QMessageBox::Ok);
            mb.setText( I_SUPPLYINCLUDE);
            mb.exec();
            return;
        }
        // a class must be specified
        if ( clss.isEmpty() ) {
            mb.setStandardButtons( QMessageBox::Ok);
            mb.setText( I_SUPPLYCLASS);
            mb.exec();
            return;
        }
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

        // add the template to the used-library list
        m_fi->addLibrary( g_findTemplate( incl));

        // create the declaration object
        ObjectWid* ow;
        if ( ui->rbActuator->isChecked() ) {
            ow = new ObjectWid( m_fi, FOT_ACTUATOR);
            m_fi->createObject( ow, name);
        }
        if ( ui->rbSensor->isChecked() ) {
            ow = new ObjectWid( m_fi, FOT_SENSOR);
            m_fi->createObject( ow, name);
        }
        ow->setDeclaration( clss, vnam, incl);

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
    ui->lblType->setEnabled( type == ADD_VARIABLE);
    ui->cbType->setEnabled( type == ADD_VARIABLE);
    if ( type == ADD_VARIABLE ) { ui->cbVariable->hide(); ui->leVariable->show(); ui->leVariable->setFocus(); }
    if ( type == DEL_VARIABLE ) { ui->cbVariable->show(); ui->leVariable->hide(); ui->cbVariable->setFocus(); }

    // device
    ui->lblDevice->setEnabled( type == ADD_DEVICE || type == DEL_DEVICE);
    ui->cbDevice->setEnabled( type == ADD_DEVICE || type == DEL_DEVICE);
    ui->leDevice->setEnabled( type == ADD_DEVICE || type == DEL_DEVICE);
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

void DeviceDlg::on_rbVariable_clicked()
{
    enable( ADD_VARIABLE);
}

void DeviceDlg::on_rbActuator_clicked()
{
    enable( ADD_DEVICE);
    ui->cbClass->clear();
    ui->cbUsing->clear();
    ui->leInclude->setText( "");
    ui->leDeclare->setText( "");
    for ( int i = 0; i < g_templates.size(); i++ ) {
        if ( g_templates.at( i)->fot() != FOT_ACTUATOR )
            continue;
        QString cls = g_templates.at( i)->getClass();
        if ( ui->cbClass->findText( cls) == -1 )
            ui->cbClass->addItem( cls);
    }
}

void DeviceDlg::on_rbSensor_clicked()
{
    enable( ADD_DEVICE);
    ui->cbClass->clear();
    ui->cbUsing->clear();
    ui->leInclude->setText( "");
    ui->leDeclare->setText( "");
    for ( int i = 0; i < g_templates.size(); i++ ) {
        if ( g_templates.at( i)->fot() != FOT_SENSOR )
            continue;
        QString cls = g_templates.at( i)->getClass();
        if ( ui->cbClass->findText( cls) == -1 )
            ui->cbClass->addItem( cls);
    }
}

void DeviceDlg::on_rbRoutineDel_clicked()
{
    enable( DEL_ROUTINE);
}

void DeviceDlg::on_rbVariableDel_clicked()
{
    enable( DEL_VARIABLE);
}

void DeviceDlg::on_rbDeviceDel_clicked()
{
    enable( DEL_DEVICE);
}

void DeviceDlg::on_cbVariable_currentTextChanged(const QString &arg1)
{

}

void DeviceDlg::on_cbDevice_currentTextChanged(const QString &arg1)
{

}

void DeviceDlg::on_cbClass_currentTextChanged(const QString &arg1)
{
    QString incl;
    ui->cbUsing->clear();
    ui->leInclude->setText( "");
    ui->leDeclare->setText( "");
    for ( int i = 0; i < g_templates.size(); i++ ) {
        Template* templ = g_templates.at( i);
        if ( templ->getClass() == arg1 ) {
            QString use = templ->getUsing();
            incl = templ->getInclude();
            if ( !use.isEmpty() && (ui->cbUsing->findText( use) == -1) )
                ui->cbUsing->addItem( use);
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
    for ( int i = 0; i < g_templates.size(); i++ ) {
        Template* templ = g_templates.at( i);
        if ( templ->getUsing() == arg1 )
            ui->leInclude->setText( templ->getInclude());
    }
}
