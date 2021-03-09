#include "language.h"
#include "flowit.h"
#include "ui_flowit.h"
#include <QPainter>
#include <QMoveEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPdfWriter>
#include <QDir>
#include <QStandardPaths>
#include "column.h"
#include "flowobject.h"
#include "flowcheck.h"
#include "flowwhile.h"
#include "flowrepeat.h"
#include "flowwait.h"
#include "flowdo.h"
#include "flowactuator.h"
#include "flowsensor.h"
#include "flowpage.h"
#include "devicedlg.h"
#include "objectwid.h"
#include "filloutdlg.h"
#include "askdlg.h"
#include "exportdlg.h"

QString TITLE = "FlowIt 1.0 - ";

#define YOFFS 70
#define XOFFS 210

#define STANDARDOBJECTS 6

bool                g_pan = false;
int                 g_panX = -1;
int                 g_panY = -1;
int                 g_panChartX = 0;
int                 g_panChartY = 0;
int                 g_panCodeX = 0;
int                 g_panCodeY = 0;

bool                g_dragging = false;
ObjectWid*          g_owDrag = NULL;
FlowObject*         g_foDrag = NULL;
FlowObject*         g_fo = NULL;
int                 g_dotType = DT_OBJECT;

QList<Column*>      g_tools;
FlowObject*         g_foTool = NULL;

QList<TVariable*>   g_variables;
QList<Template*>    g_templates;
QList<Template*>    g_libraries;
QList<ObjectWid*>   g_objects;

QString             g_tabs;
FlowPage*           g_rootPage = NULL;

Template* g_findTemplate( QString include)
{
    Template* templ;
    for ( int i = 0; i < g_templates.size(); i++ ) {
        templ = g_templates.at( i);
        if ( templ->getInclude() == include )
            return templ;
    }
}

void g_parseTag( QString ln, QString& tag, QString& val)
{
    int ix;
    tag = "";
    val = "";
    ln = ln.trimmed();
    if ( ln.isEmpty() || ln.at( 0) != '<' || (ix = ln.indexOf( '>')) < 0 )
        return;
    tag = ln.left( ix + 1).trimmed().toLower();
    ln = ln.right( ln.length() - ix - 1).trimmed();
    if ( (ix = ln.indexOf( "</")) < 0 || ln.indexOf( '>') < 0 )
        return;
    val = ln.left( ix).trimmed();
}

bool g_validName( QString name)
{
    if ( name.isEmpty() ) return false;
    QString white = " `~!@#$%^&*()-+=[]{}|\\:;\"'<>,.?/";
    for ( int i = 0; i < white.length(); i++ ) {
        if ( name.indexOf( white.at( i)) >= 0 )
            return false;
    }
    return true;
}

FlowIt::FlowIt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FlowIt)
{
    ui->setupUi(this);

    setLanguage();

    centralWidget()->setAttribute( Qt::WA_TransparentForMouseEvents);
    setMouseTracking( true);
    setWindowState( Qt::WindowMaximized);

    getLatestProject();
    if ( m_savePath.isEmpty() ) {
        clear();
    }
    else
        openProject();

    grabKeyboard();
}

FlowIt::~FlowIt()
{
    delete ui;
}

bool FlowIt::declarationExists( QString declaration)
{
    for ( int i = 0; i < g_tools.size(); i++ )
        if ( g_tools.at( i)->root()->name() == declaration )
            return true;
    for ( int i = 0; i < g_objects.size(); i++ ) {
        if ( g_objects.at( i)->getInstance() == declaration )
            return true;
        if ( g_objects.at( i)->name() == declaration )
            return true;
    }
    for ( int i = 0; i < g_variables.size(); i++ )
        if ( g_variables.at( i)->name == declaration )
            return true;
    return false;
}

void FlowIt::createTool( FlowObject* foTool)
{
    QAction* action = new QAction( ui->toolBar);
    action->setText( foTool->name());
    ui->toolBar->addAction( action);
    ui->toolBar->connect( action, SIGNAL(triggered()), this, SLOT(on_routine_triggered()));
    foTool->setAction( action);

    Column* cl = (Column*) foTool->column();
    FlowEnd* fe = new FlowEnd( foTool->column());
    fe->setChartText( foTool->chartText());
    foTool->attachAfter( fe);
    g_tools.append( cl);
    m_column = cl;
}
bool FlowIt::deleteTool( QString name)
{
    FlowObject* fo = NULL;
    int ix;
    for ( ix = 0; ix < g_tools.size(); ix++ ) {
        fo = g_tools.at( ix)->root();
        if ( fo->name() == name )
            break;
    }
    if ( !fo ) return false; // should never occur

    QMessageBox mb;
    mb.setStandardButtons( QMessageBox::Yes | QMessageBox::No);
    mb.setText( fo->type() == FOT_ROUTINE ? I_DELROUTINE : I_DELPAGE);
    releaseKeyboard();
    if ( mb.exec() == QMessageBox::Yes ) {
        g_tools.removeAt( ix);
        QAction* action = fo->getAction();
        ui->toolBar->removeAction( action);
        delete action;
        if ( fo->column() == m_column )
            m_column = g_tools.at( 1);
        delete fo->column();
        g_foTool = NULL;
        grabKeyboard();
        return true;
    }
    grabKeyboard();
    return false;
}

void FlowIt::createVariable( QString type, QString name)
{
    TVariable* var = new TVariable;
    var->name = name;
    var->type = type;
    g_variables.append( var);
    repaint();
}

bool FlowIt::deleteVariable( QString name)
{
    // check if the variable is still used by the flowchart
    if ( hasVariable( name) ) {
        QMessageBox mb;
        mb.setText( I_OBJECTINUSE);
        releaseKeyboard();
        mb.exec();
        grabKeyboard();
        return false;
    }
    for ( int i = 0; i < g_variables.size(); i++ ) {
        TVariable* tv = g_variables.at( i);
        if ( tv->name == name ) {
            g_variables.removeAt( i);
            delete tv;
            repaint();
            return true;
        }
    }
    return false;
}

void FlowIt::createObject( ObjectWid* ow, QString name)
{
    ow->init( m_yObj, name);
    ow->show();
    g_objects.append( ow);
    listObjects();
}

bool FlowIt::deleteObject( QString name)
{
    // find the object
    int i, ix;
    for ( i = 6; i < g_objects.size(); i++ )
        if ( g_objects.at( i)->name() == name )
            break;
    if ( i >= g_objects.size() )
        return false;

    // check if the object is still used by the flowchart
    if ( hasInstance( g_objects.at( i)->getInstance()) ) {
        QMessageBox mb;
        mb.setText( I_OBJECTINUSE);
        releaseKeyboard();
        mb.exec();
        grabKeyboard();
        return false;
    }

    // check if the object's template is used anywhere else
    Template* templ = g_objects.at( i)->getTemplate();
    for ( ix = 0; ix < g_objects.size(); ix++ ) {
        if ( g_objects.at( ix)->getTemplate() == templ )
            break;
    }

    // if no longer in use, remove the template from the libraries
    if ( ix < g_objects.size() ) {
        for ( int j = 0; j < g_libraries.size(); j++ ) {
            if ( g_libraries.at( j) == templ ) {
                g_libraries.removeAt( j);
                break;
            }
        }
    }

    // delete the object declaration
    delete g_objects.at( i);
    g_objects.removeAt( i);
    listObjects();
    saveProject();
    repaint();

    return true;
}

void FlowIt::listObjects()
{
    m_yObj = YOFFS;
    for ( int i = 0; i < g_objects.size(); i ++ ) {
        ObjectWid* ow = g_objects.at( i);
        ow->move( DRAG_HOME, m_yObj);
        m_yObj += ow->height() + CONNECT_HEIGHT;
    }
}

bool FlowIt::hasInstance( QString name)
{
    for ( int i = 0; i < g_tools.size(); i++ )
        if ( g_tools.at( i)->hasInstance( name) )
            return true;
    return false;
}

bool FlowIt::hasVariable( QString name)
{
    for ( int i = 0; i < g_tools.size(); i++ )
        if ( g_tools.at( i)->hasVariable( name) )
            return true;
    return false;
}

void FlowIt::addLibrary( Template* templ)
{
    for ( int i = 0; i < g_libraries.size(); i++ )
        if ( g_libraries.at( i) == templ )
            return;
    g_libraries.append( templ);
}

void FlowIt::clear()
{
    // clear all

    for ( int i = 0; i < g_templates.size(); i++ )
        delete g_templates.at( i);
    g_templates.clear();
    g_libraries.clear();

    for ( int i = 0; i < g_objects.size(); i++ )
        delete g_objects.at( i);
    g_objects.clear();

    for ( int i = 0; i < g_tools.size(); i++ )
        delete g_tools.at( i);
    g_tools.clear();

    ui->toolBar->clear();

    // create toolbar without routines

    QAction* action;

    action = new QAction( ui->toolBar);
    action->setIcon( QIcon( ":/file.png"));
    ui->toolBar->addAction( action);
    ui->toolBar->connect( action, SIGNAL(triggered()), this, SLOT(on_file_triggered()));

    if ( m_savePath.isEmpty() ) return;

    action = new QAction( ui->toolBar);
    action->setIcon( QIcon( ":/export.png"));
    ui->toolBar->addAction( action);
    ui->toolBar->connect( action, SIGNAL(triggered()), this, SLOT(on_export_triggered()));

    ui->toolBar->addSeparator();

    m_panchart = new QAction( ui->toolBar);
    m_panchart->setIcon( QIcon( ":/panon.png"));
    ui->toolBar->addAction( m_panchart);
    ui->toolBar->connect( m_panchart, SIGNAL(triggered()), this, SLOT(on_pan_triggered()));

    m_drawchart = new QAction( ui->toolBar);
    m_drawchart->setIcon( QIcon( ":/panoff.png"));
    ui->toolBar->connect( m_drawchart, SIGNAL(triggered()), this, SLOT(on_pan_triggered()));

    m_adddev = new QAction( ui->toolBar);
    m_adddev->setIcon( QIcon( ":/device.png"));
    ui->toolBar->addAction( m_adddev);
    ui->toolBar->connect( m_adddev, SIGNAL(triggered()), this, SLOT(on_device_triggered()));

    ui->toolBar->addSeparator();

    // create the standard object windows

    m_yObj = YOFFS;
    ObjectWid* ow;
    ow = new ObjectWid( this, FOT_PAGE);
    createObject( ow, FO_PAGE);
    ow = new ObjectWid( this, FOT_CHECK);
    createObject( ow, FO_CHECK);
    ow = new ObjectWid( this, FOT_WHILE);
    createObject( ow, FO_WHILE);
    ow = new ObjectWid( this, FOT_REPEAT);
    createObject( ow, FO_REPEAT);
    ow = new ObjectWid( this, FOT_WAIT);
    createObject( ow, FO_WAIT);
    ow = new ObjectWid( this, FOT_DO);
    createObject( ow, FO_DO);

    // read the standard templates

    QDir dir;
    QStringList devs;
    QString path;
    devs << "*.fit";
    dir.setNameFilters( devs);
    dir.setPath( "Templates");
    path = dir.absolutePath();
    devs = dir.entryList();
    for ( int i = 0; i < devs.size(); i++ ) {
        Template* templ = new Template;
        templ->import( path + "/" + devs.at( i));
        g_templates.append( templ);
    }
}

void FlowIt::paintChart( QPainter& pi, Column* column, int x, int y, bool print)
{
    if ( !print ) {
        if ( g_pan && (g_panX >= 0) ) {
            pi.setPen( QColor( 255, 160, 64));
            pi.drawLine( mouseX, mouseY, g_panX, g_panY);
        }
    }
    column->drawChart( pi, x, y);
}

void FlowIt::paintCode( QPainter& pi, int x, int y, int h)
{
    // start of code drawing
    pi.setPen( Qt::black);
    pi.setFont( QFont( "Arial", 8));
    int dummy;
    FlowObject::calcSize( "hj", dummy, h);

    // draw includes and usings
    for ( int i = 0; i < g_libraries.size(); i++ ) {
        QString incl = g_libraries.at( i)->getInclude();
        QString use = g_libraries.at( i)->getUsing();
        pi.drawText( QRect( x, y, 1000, h), "#include <" + incl + ">");
        y += h;
        if ( !use.isEmpty() ) {
            pi.drawText( QRect( x, y, 1000, h), "using namespace " + use + ";");
            y += h;
        }
    }
    if ( g_libraries.size() ) y += h;

    // draw declarations
    for ( int i = STANDARDOBJECTS; i < g_objects.size(); i++ ) {
        ObjectWid* ow = g_objects.at( i);
        QString decl = ow->getTemplate()->getClass() + " " + ow->getInstance() + ";";
        if ( !ow->getTemplate()->getUsing().isEmpty() )
            decl = ow->getTemplate()->getUsing() + "::" + decl;
        pi.drawText( QRect( x, y, 1000, h), decl);
        y += h;
    }
    if ( g_objects.size() > 3 ) y += h;

    // draw variables
    for ( int i = 0; i < g_variables.size(); i++ ) {
        QString decl = g_variables.at( i)->type + " " + g_variables.at( i)->name + ";";
        pi.drawText( QRect( x, y, 1000, h), decl);
        y += h;
    }
    if ( g_variables.size() ) y += h;

    // draw routines
    for ( int i = 2; i < g_tools.size(); i++ ) { // first all extra routines
        if ( g_tools.at( i)->root()->type() == FOT_ROUTINE )
            g_tools.at( i)->drawCode( pi, h, x, y);
    }                                            // then:
    g_tools.at( 0)->drawCode( pi, h, x, y);      // setup()
    g_tools.at( 1)->drawCode( pi, h, x, y);      // loop()
}

void FlowIt::exportCode( QTextStream& out)
{
    g_tabs = "";

    // output includes and usings
    for ( int i = 0; i < g_libraries.size(); i++ ) {
        QString incl = g_libraries.at( i)->getInclude();
        QString use = g_libraries.at( i)->getUsing();
        out << "#include <" + incl + ">\n";
        if ( !use.isEmpty() )
            out << "using namespace " + use + ";\n";
    }
    out << "\n";

    // output declarations
    for ( int i = STANDARDOBJECTS; i < g_objects.size(); i++ ) {
        QString decl = g_objects.at( i)->getTemplate()->getClass() + " " + g_objects.at( i)->getInstance() + ";\n";
        out << decl;
    }
    out << "\n";

    // output routines
    for ( int i = 2; i < g_tools.size(); i++ ) { // first all extra routines
        if ( g_tools.at( i)->root()->type() == FOT_ROUTINE )
            g_tools.at( i)->exportCode( out);
    }                                            // then:
    g_tools.at( 0)->exportCode( out);            // setup()
    g_tools.at( 1)->exportCode( out);            // loop()
}

void FlowIt::paintEvent(QPaintEvent *)
{
    if ( m_savePath.isEmpty() ) {
        QImage img;
        img.load( "fistartup.png");
        QPainter pw( this);
        pw.drawImage( 120, 120, img);
        return;
    }

    // it is advised to use QImage as an intermediate
    // painting device in order to be sure to get identical
    // results on different real devices

    g_foTool = NULL;
    g_fo = NULL;
    int w = m_column->leftWidth();
    int h = m_column->height();

    QImage img( 2000, 1500, QImage::Format_ARGB32);
    QPainter pi( &img);

    // draw drag circles near the objects in the list
    pi.setPen( Qt::darkGreen);
    int y = YOFFS;
    for ( int i = 0; i < g_objects.size(); i++ ) {
        int yp = y + g_objects.at( i)->height() / 2;
        pi.drawEllipse( DRAG_HOME + 105, yp - 2, 4, 4);
        y += g_objects.at( i)->height() + CONNECT_HEIGHT;
    }

    // draw separator between object list and flow chart
    pi.setPen( Qt::gray);
    pi.drawLine( XOFFS - 36, YOFFS - 20, XOFFS - 36, 3000);

    // draw paper
    pi.setClipRect( XOFFS - 30, YOFFS - 20, g_panChartX + m_column->width() + 65, 3000);
    pi.fillRect( g_panChartX + XOFFS - 57, g_panChartY + YOFFS - 57, 800, 1130, QColor( 255, 255, 240));
    pi.setClipRect( 0, 0, 3000, 3000);

    // draw flow chart
    pi.setClipRect( XOFFS - 30, YOFFS - 20, 3000, 3000);
    paintChart( pi, m_column, g_panChartX + XOFFS + w/2, g_panChartY + YOFFS);

    // draw separator between flow chart and code
    w = m_column->width();
    pi.setPen( Qt::gray);
    pi.drawLine( g_panChartX + w + XOFFS + 40, YOFFS - 20, g_panChartX + w + XOFFS + 40, 3000);

    // draw code
    pi.setClipRect( g_panChartX + w + XOFFS + 50, YOFFS - 20, 3000, 3000);
    paintCode( pi, g_panCodeX + g_panChartX + w + XOFFS + 80, g_panCodeY + YOFFS, h);

    QPainter pw( this);
    pw.drawImage( 0, 0, img);
}

void FlowIt::mouseMoveEvent(QMouseEvent * event)
{
    repaint();

    if ( g_pan ) {
        if ( g_panX >= 0 ) {
            g_panX = event->pos().x();
            g_panY = event->pos().y();
        }
        return;
    }

    mouseX = event->pos().x();
    mouseY = event->pos().y();
    if ( g_owDrag )
        g_owDrag->move( mouseX - 105, mouseY - g_owDrag->height() / 2);
    if ( (g_fo || g_owDrag) && (event->buttons() & Qt::LeftButton ) )
        g_dragging = true;
}

void FlowIt::mousePressEvent(QMouseEvent * event)
{
    mouseX = event->pos().x();
    mouseY = event->pos().y();

    if ( g_pan ) {
        g_panX = mouseX;
        g_panY = mouseY;
        return;
    }

    g_owDrag = NULL;
    g_foDrag = g_fo;
    for ( int i = 0; i < g_objects.size(); i++ )
        if ( g_objects.at( i)->inDrag( mouseX, mouseY) ) {
            g_owDrag = g_objects.at( i);
            break;
        }
    repaint();
}

void FlowIt::mouseReleaseEvent(QMouseEvent * event)
{
    if ( g_pan ) {
        g_panX = -1;
        g_panY = -1;

        if ( mouseX < XOFFS ) {
            g_panChartX = 0;
            g_panChartY = 0;
            g_panCodeX = 0;
            g_panCodeY = 0;
        }
        else
        if ( mouseX < g_panChartX + m_column->width() + 250 ) {
            g_panChartX += event->pos().x() - mouseX;
            g_panChartY += event->pos().y() - mouseY;
        }
        else {
            g_panCodeX += event->pos().x() - mouseX;
            g_panCodeY += event->pos().y() - mouseY;
        }
        repaint();
        return;
    }

    mouseX = event->pos().x();
    mouseY = event->pos().y();

    if ( g_owDrag ) {

        //
        // insert a new flowobject
        //

        g_owDrag->home();
        if ( g_fo ) {
            FilloutDlg dlg;
            AskDlg ask;
            FlowObject* fo;
            Column* column;

            // determine the column for the flowobject
            switch ( g_dotType ) {
                case DT_OBJECT:     column = m_column;
                                    break;
                case DT_LEFT:       if ( g_fo->type() == FOT_CHECK )
                                        column = ((FlowCheck*)g_fo)->columnLeft();
                                    else
                                        column = ((FlowRepeat*)g_fo)->column();
                                    break;
                case DT_RIGHT:      column = ((FlowCheck*)g_fo)->columnRight();
                                    break;
                default:            column = (Column*) g_fo->column();
                                    break;
            }

            // create the new flowobject, meanwhile
            // prepare the instance field of the attribute dialog
            switch ( g_owDrag->type() ) {
                case FOT_CHECK:     fo = new FlowCheck( column, this); break;
                case FOT_WHILE:     fo = new FlowWhile( column, this); break;
                case FOT_REPEAT:    fo = new FlowRepeat( column, this);
                                    ask.setLabel( FO_TIMES);
                                    ask.setValue( 10);
                                    break;
                case FOT_PAGE:      {
                                    if ( m_column->root()->type() == FOT_PAGE ) {
                                        QMessageBox msg;
                                        msg.setWindowTitle( T_NOTALLOWED);
                                        msg.setText( I_NOPAGENESTING);
                                        releaseKeyboard();
                                        msg.exec();
                                        grabKeyboard();
                                        g_owDrag = NULL;
                                        g_foDrag = NULL;
                                        g_dragging = false;
                                        g_fo = NULL;
                                        return;
                                    }
                                    QString pg = QString::number( FlowPage::freePageNr());
                                    fo = new FlowPage( column);
                                    if ( g_rootPage )
                                        g_rootPage->setNextPage( (FlowPage*) fo);
                                    else
                                        g_rootPage = (FlowPage*) fo;
                                    fo->setChartText( pg);
                                    Column* col = new Column;
                                    FlowPage* fp = new FlowPage( col);
                                    fp->init( V_PAGE + pg);
                                    fp->setChartText( pg);
                                    col->setMain( this);
                                    col->setRoot( fp);
                                    createTool( fp);
                                    break;
                                    }
                case FOT_WAIT:      fo = new FlowWait( column);
                                    ask.setLabel( FO_MSEC);
                                    ask.setValue( 1000);
                                    break;
                case FOT_DO:        {
                                    fo = new FlowDo( column);
                                    dlg.addInstance( ""); // means no instances
                                    QList<QAction*> al = ui->toolBar->actions();
                                    for ( int i = 11; i < al.size(); i++ )
                                        dlg.addRoutine( al.at( i)->text() + "()");
                                    break;
                                    }
                case FOT_ACTUATOR:  fo = new FlowActuator( column);
                                    ((FlowActuator*)fo)->init( g_owDrag->name(), g_owDrag->getInstance(), g_owDrag->getTemplate());
                                    dlg.addInstance( g_owDrag->getInstance());
                                    break;
                case FOT_SENSOR:    fo = new FlowSensor( column);
                                    ((FlowSensor*)fo)->init( g_owDrag->name(), g_owDrag->getInstance(), g_owDrag->getTemplate());
                                    dlg.addInstance( g_owDrag->getInstance());
                                    break;
            }

            if ( g_owDrag->type() == FOT_CHECK || g_owDrag->type() == FOT_WHILE ) {
                for ( int i = STANDARDOBJECTS; i < g_objects.size(); i++ )
                    dlg.addInstance( g_objects.at( i)->getInstance());
            }

            // position the flowobject in the chart
            switch ( g_dotType ) {
                case DT_OBJECT:     break;
                case DT_CONNECT:    g_fo->attachAfter( fo);
                                    break;
                default:            column->insertRoot( fo);
                // 'column' is determined above, depends on connection type
                                    break;
            }

            if ( g_owDrag->type() == FOT_REPEAT || g_owDrag->type() == FOT_WAIT ) {
                releaseKeyboard();
                ask.exec();
                grabKeyboard();
                int val = ask.value();
                QString txt = (val == -1 ? "" : QString::number( val));
                fo->setChartText( txt);
                fo->setCodeText( txt);
            }
            else {
                if ( g_owDrag->type() != FOT_PAGE ) {
                    releaseKeyboard();
                    dlg.exec();
                    grabKeyboard();
                    fo->setChartText( dlg.chartText());
                    fo->setCodeText( dlg.codeText());
                }
            }

            saveProject();
        }
    }
    else {
        if ( g_fo ) {
            if ( g_dragging && (g_foDrag != g_fo) ) {

                //
                // reposition an existing flowobject
                //

                if ( (g_fo->column() != g_foDrag->column()) && // exclude from 'isChildOf'
                     ((Column*)g_fo->column())->isChildOf( (Column*)g_foDrag->column() ) ) {
                    // may not reposition a check/repeat object into its own subcolumns
                    QMessageBox msg;
                    msg.setWindowTitle( T_NOTALLOWED);
                    msg.setText( I_NOMOVEINSUB);
                    releaseKeyboard();
                    msg.exec();
                    grabKeyboard();
                }
                else {
                    g_foDrag->detach();
                    switch ( g_dotType ) {
                        case DT_LEFT:   if ( g_fo->type() == FOT_CHECK )
                                            ((FlowCheck*)g_fo)->columnLeft()->insertRoot( g_foDrag);
                                        else
                                        if ( g_fo->type() == FOT_WHILE )
                                            ((FlowWhile*)g_fo)->column()->insertRoot( g_foDrag);
                                        else
                                            ((FlowRepeat*)g_fo)->column()->insertRoot( g_foDrag);
                                        break;
                        case DT_RIGHT:  ((FlowCheck*)g_fo)->columnRight()->insertRoot( g_foDrag);
                                        break;
                        default:        g_fo->attachAfter( g_foDrag); break;
                    }

                    saveProject();
                }

            }
            else {

                //
                // modify the attributes of an existing flowobject
                //

                FilloutDlg dlg;
                AskDlg ask;
                if ( g_fo->type() == FOT_ACTUATOR )
                    dlg.addInstance( ((FlowActuator*)g_fo)->getInstance());
                else
                if ( g_fo->type() == FOT_SENSOR )
                    dlg.addInstance( ((FlowSensor*)g_fo)->getInstance());
                else
                if ( g_fo->type() == FOT_DO )
                    dlg.addInstance( ""); // means no instance
                else
                    for ( int i = STANDARDOBJECTS; i < g_objects.size(); i++ )
                        dlg.addInstance( g_objects.at( i)->getInstance());

                if ( g_fo->type() == FOT_REPEAT || g_fo->type() == FOT_WAIT ) {
                    int val = g_fo->getValue();
                    if ( !val )
                        val = (g_fo->type() == FOT_REPEAT ? 10 : 1000);
                    ask.setValue( val);
                    ask.setLabel( g_fo->type() == FOT_REPEAT ? FO_TIMES : FO_MSEC);
                    releaseKeyboard();
                    ask.exec();
                    grabKeyboard();
                    val = ask.value();
                    QString txt = (val == -1 ? "" : QString::number( val));
                    g_fo->setChartText( txt);
                    g_fo->setCodeText( txt);
                }
                else
                if ( g_fo->type() != FOT_PAGE ) {
                    dlg.setChartText( g_fo->chartText());
                    dlg.setCodeText( g_fo->codeText());
                    releaseKeyboard();
                    dlg.exec();
                    grabKeyboard();
                    g_fo->setChartText( dlg.chartText());
                    g_fo->setCodeText( dlg.codeText());
                }

                saveProject();
            }
        }
    }

    g_owDrag = NULL;
    g_foDrag = NULL;
    g_dragging = false;
    g_fo = NULL;

    repaint();
}

void FlowIt::keyPressEvent(QKeyEvent * event)
{
    if ( (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) ) {

        for ( int i = 0; i < g_objects.size(); i++ ) {
            if ( g_objects.at( i)->selected() ) {
                deleteObject( g_objects.at( i)->name());
                return;
            }
        }

        if ( g_fo ) {
            if ( g_fo->type() == FOT_PAGE ) {
                QString name = V_PAGE + g_fo->chartText();
                if ( !deleteTool( name) )
                    return;
                FlowPage::removePage( (FlowPage*) g_fo);
            }
            g_fo->detach();
            g_fo->destroy();
            delete g_fo;
            g_fo = NULL;
            saveProject();
            repaint();
        }

        if ( g_foTool  ) {
            // a tool of FOT_PAGE type may not be deleted
            // will be deleted automatically with the page link in the chart
            if ( g_foTool->type() == FOT_PAGE ) {
                QMessageBox mb;
                mb.setText( I_NODELPAGE);
                releaseKeyboard();
                mb.exec();
                grabKeyboard();
                return;
            }

            // setup and loop routine may not be deleted
            QString name = g_foTool->name();
            if ( name == "setup" || name == "loop" ) {
                QMessageBox mb;
                mb.setText( I_NODELSETUPORLOOP);
                releaseKeyboard();
                mb.exec();
                grabKeyboard();
                return;
            }

            deleteTool( g_foTool->name());
        }
    }
}

void FlowIt::on_file_triggered()
{
    QString mydocs = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation);
    QFileDialog fd( this, T_OPENPROJECT, mydocs, V_FIPFILES);
    fd.setDefaultSuffix( "fip");
    fd.setOption( QFileDialog::DontUseNativeDialog);
    fd.setAcceptMode( QFileDialog::AcceptOpen);
    releaseKeyboard();
    if ( fd.exec() == QDialog::Rejected ) {
        grabKeyboard();
        return;
    }
    grabKeyboard();
    saveProject();
    m_savePath = fd.selectedFiles().first();
    clear();
    if ( QFile::exists( m_savePath) )
        openProject();
    else
        newProject();
    setLatestProject();
    return;
}

void FlowIt::newProject()
{
    setWindowTitle( TITLE + m_savePath);

    clear();

    FlowRoutine* fr;
    Column* col;

    col = new Column;
    col->setMain( this);
    fr = new FlowRoutine( col);
    fr->init( "setup");
    createTool( fr);

    col = new Column;
    col->setMain( this);
    fr = new FlowRoutine( col);
    fr->init( "loop");
    createTool( fr);

    m_column = g_tools.at( 0);

    saveProject();
    repaint();
}

void FlowIt::openProject()
{
    setWindowTitle( TITLE + m_savePath);

    clear();

    QFile file( m_savePath);
    if (!file.open( QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in( &file);
    QString ln, tag, val;
    TVariable* tv = NULL;

    while ( !in.atEnd() ) {
        ln = in.readLine();
        g_parseTag( ln, tag, val);

        // READ VARIABLE

        if ( tag == "<variable>" ) {
            tv = new TVariable;
            tv->type = -1;
            continue;
        }
        else
        if ( tag == "</variable>" ) {
            if ( tv ) {
                if ( (tv->type >= 0) && !tv->name.isEmpty() )
                    g_variables.append( tv);
                else
                    delete tv;
                tv = NULL;
            }
            continue;
        }

        if ( tv )  {    // within variable declaration
            if ( tag == "<name>" )
                tv->name = val;
            if ( tag == "<type>" )
                tv->type = val;
            continue;
        }

        // READ TEMPLATE

        if ( tag == "<template>" ) {
            Template* templ = new Template;
            if ( templ->read( in) )
                g_templates.append( templ);
            else
                delete templ;
            continue;
        }

        // READ DECLARATION

        if ( tag == "<declaration>" ) {
            ObjectWid* ow = new ObjectWid( this);
            createObject( ow, "");
            ow->read( in);
            Template* templ = ow->getTemplate();
            addLibrary( templ);
            continue;
        }

        // READ CHART AND CODE

        if ( tag == "<column>" ) {
            Column* col = new Column;
            col->setMain( this);
            col->read( in);
        }
    }

    file.close();

    if ( !g_tools.size() )
        newProject();
    else
        m_column = g_tools.at( 0);
}

void FlowIt::saveProject()
{
    if ( m_savePath.isEmpty() ) return;

    QFile file( m_savePath);
    if (!file.open( QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out( &file);

    for ( int i = 0; i < g_variables.size(); i++ ) {
        TVariable* tv = g_variables.at( i);
        out << "<variable>\n\t<name>" + tv->name + "</name>\n";
        out << "\t<type>" + tv->type + "</type>\n</variable>\n\n";
    }

    QList<Template*> tl;
    for ( int i = 0; i < g_objects.size(); i++ ) {
        Template* templ = g_objects.at( i)->getTemplate();
        if ( templ && (tl.indexOf( templ) < 0) )
            tl.append( templ);
    }
    for ( int i = 0; i < tl.size(); i++ )
        tl.at( i)->write( out);
    for ( int i = STANDARDOBJECTS; i < g_objects.size(); i++ )
        g_objects.at( i)->write( out);
    for ( int i = 0; i < g_tools.size(); i++ ) {
        g_tools.at( i)->write( out);
        out << "\n";
    }

    file.close();
}

void FlowIt::on_export_triggered()
{
    ExportDlg dlg;
    QString msg;
    releaseKeyboard();
    if ( dlg.exec() == QDialog::Rejected ) {
        grabKeyboard();
        return;
    }

    if ( dlg.exportType() == EX_ARDUINO || dlg.exportType() == EX_RASPBERRY ) {
        QFile file( dlg.filePath());
        if (!file.open( QIODevice::WriteOnly | QIODevice::Text)) {
            msg = I_CODEFAILURE;
            goto export_msg;
        }
        QTextStream out( &file);
        exportCode( out);
        file.close();
        msg = I_CODESUCCESS;
    }
    else
    if ( dlg.exportType() == EX_FLOWCHART ) {
        QPdfWriter pdf( dlg.filePath());
        QPainter pw;
        pdf.setResolution( 100);
        if ( !pw.begin( &pdf) ) {
            msg = I_PDFFAILURE;
            goto export_msg;
        }
        for ( int i = 0; i < g_tools.size(); i++ ) {
            paintChart( pw, g_tools.at( i), 50 + m_column->leftWidth()/2, 50, true);
            if ( i < g_tools.size() - 1 )
                pdf.newPage();
        }
        pw.end();
        msg = I_PDFSUCCESS;
    }
    else
    if ( dlg.exportType() == EX_PRINTCHART ) {
        // resolution is 100 dpi
        QPrinter printer( QPrinterInfo::printerInfo( dlg.printerName()));
        QPainter pw;
        if ( !pw.begin( &printer) ) {
            msg = I_PRINTFAILURE;
            goto export_msg;
        }
        for ( int i = 0; i < g_tools.size(); i++ ) {
            paintChart( pw, g_tools.at( i), 50 + m_column->leftWidth()/2, 50, true);
            if ( i < g_tools.size() - 1 )
                printer.newPage();
        }
        pw.end();
        msg = I_PRINTSUCCESS;
    }

export_msg:
    QMessageBox mb;
    mb.setText( msg);
    mb.exec();
    grabKeyboard();
}

void FlowIt::on_device_triggered()
{
    DeviceDlg dlg;
    dlg.setFI( this);
    releaseKeyboard();
    if ( dlg.exec() == QDialog::Accepted )
        saveProject();
    grabKeyboard();
}

void FlowIt::on_pan_triggered()
{
    g_pan = !g_pan;
    if ( g_pan ) {
        ui->toolBar->removeAction( m_panchart);
        ui->toolBar->insertAction( m_adddev, m_drawchart);
    }
    else {
        ui->toolBar->removeAction( m_drawchart);
        ui->toolBar->insertAction( m_adddev, m_panchart);
    }
}

void FlowIt::on_routine_triggered()
{
    QString alias = ((QAction*) sender())->iconText();
    for ( int i = 0; i < g_tools.size(); i++ ) {
        QString n = g_tools.at( i)->root()->name();
        if ( g_tools.at( i)->root()->name() == alias ) {
            m_column = g_tools.at( i);
            break;
        }
    }
    repaint();
}

void FlowIt::setLatestProject()
{
    QFile file( "config.fi");
    if ( file.open( QIODevice::WriteOnly | QIODevice::Text) ) {
        QTextStream out( &file);
        out << m_savePath;
        file.close();
    }
}

void FlowIt::getLatestProject()
{
    QFile file( "config.fi");
    if ( file.open( QIODevice::ReadOnly | QIODevice::Text) ) {
        QString ln;
        QTextStream in( &file);
        while ( !in.atEnd() ) {
            ln = in.readLine().trimmed();
            if ( !ln.isEmpty() ) break;
        }
        file.close();
        if ( QFile::exists( ln) )
            m_savePath = ln;
    }
}
