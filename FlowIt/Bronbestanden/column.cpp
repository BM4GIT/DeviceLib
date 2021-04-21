#include "language.h"
#include "column.h"
#include "flowit.h"
#include "flowobject.h"
#include "flowactuator.h"
#include "flowcheck.h"
#include "flowdo.h"
#include "flowrepeat.h"
#include "flowroutine.h"
#include "flowwait.h"
#include "flowwhile.h"
#include "flowuntil.h"
#include "flowsensor.h"
#include "flowstorage.h"
#include "flowinterface.h"
#include "flowpage.h"
#include "template.h"

Column::Column()
{
    m_fi = NULL;
    m_parent = NULL;
    m_root = NULL;
}

Column::~Column()
{
    m_root->destroy();
    delete m_root;
}

void Column::read( QTextStream& in)
{
    FlowObject* curr = NULL;
    FlowObject* next = NULL;
    QString ln, tag, var;
    while ( !in.atEnd() ) {
        next = NULL;

        ln = in.readLine();
        g_parseTag( ln, tag, var);
        if ( tag == "</column>") break;

        if ( tag == "<flowroutine>") {
            curr = new FlowRoutine( this);
            curr->read( in);
            ((FlowIt*)m_fi)->createTool( curr);
            continue;
        }
        else
        if ( tag == "<flowpage>" ) {
            next = new FlowPage( this);
            next->read( in);
            if ( next->name().isEmpty() ) {
                // it is a page link
                if ( g_rootPage )
                    g_rootPage->setNextPage( (FlowPage*) next);
                else
                    g_rootPage = (FlowPage*) next;
                if ( curr )
                    curr->attachAfter( next);
                else // this can happen with check/repeat/while columns
                    m_root = curr = next;
            }
            else {
                // it is page tool
                next->setColumn( this);
                setRoot( next);
                ((FlowIt*)m_fi)->createTool( next);
            }

            curr = next;
            continue;
        }
        else
        if ( tag == "<flowend>" )
            next = new FlowEnd( this);
        else
        if ( tag == "<flowactuator>" )
            next = new FlowActuator( this);
        else
        if ( tag == "<flowcheck>" )
            next = new FlowCheck( this, (FlowIt*) m_fi);
        else
        if ( tag == "<flowdo>" )
            next = new FlowDo( this);
        else
        if ( tag == "<flowrepeat>" )
            next = new FlowRepeat( this, (FlowIt*) m_fi);
        else
        if ( tag == "<flowwait>" )
            next = new FlowWait( this);
        else
        if ( tag == "<flowwhile>" )
            next = new FlowWhile( this, (FlowIt*) m_fi);
        else
        if ( tag == "<flowuntil>" )
            next = new FlowUntil( this, (FlowIt*) m_fi);
        else
        if ( tag == "<flowsensor>" )
            next = new FlowSensor( this);
        else
        if ( tag == "<flowstorage>" )
            next = new FlowStorage( this);
        else
        if ( tag == "<flowinterface>" )
            next = new FlowInterface( this);

        next->read( in);

        if ( !m_root )
            m_root = curr = next;
        else {
            curr->attachAfter( next);
            curr = next;
        }
    }
}

void Column::write( QTextStream& out)
{
    out << g_tabs + "<column>\n";
    g_tabs += '\t';
    FlowObject* curr = m_root;
    while ( curr ) {
        curr->write( out);
        curr = curr->next();
    }
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << g_tabs + "</column>\n";
}

void Column::setMain( void* fi)
{
    m_fi = fi;
}

void Column::setParent( Column *parent)
{
    m_parent = parent;
}

bool Column::isChildOf( Column* column)
{
    if ( this == column ) return true;
    if ( !m_parent ) return false;
    return m_parent->isChildOf( column);
}

bool Column::hasInstance( QString name)
{
    if ( !m_root ) return false;
    return m_root->hasInstance( name);
}

bool Column::hasVariable( QString name)
{
    if ( !m_root ) return false;
    return m_root->hasVariable( name);
}

void Column::setRoot( FlowObject* fo)
{
    m_root = fo;
}

void Column::insertRoot( FlowObject* fo)
{
    if ( m_root )
        m_root->attachBefore( fo);
    else
        m_root = fo;
    fo->setColumn( this);
}

FlowObject* Column::root()
{
    return m_root;
}

int Column::width()
{
    int w = 0;
    FlowObject* curr = m_root;
    while ( curr ) {
        if ( w < curr->width() )
            w = curr->width();
        curr = curr->next();
    }
    return w;
}

int Column::leftWidth()
{
    int w = 0;
    FlowObject* curr = m_root;
    while ( curr ) {
        if ( w < curr->leftWidth() )
            w = curr->leftWidth();
        curr = curr->next();
    }
    return w;
}

int Column::height()
{
    int h = 0;
    FlowObject* curr = m_root;
    while ( curr ) {
        h += curr->height() + CONNECT_HEIGHT;
        curr = curr->next();
    }
    return h;
}

void Column::drawChart( QPainter& p, int x, int& y)
{
    FlowObject* curr = m_root;
    int w = leftWidth();
    while ( curr ) {
        curr->setPosition( x, y);
        curr->drawChart( p);
        y += curr->height() + CONNECT_HEIGHT;
        if ( curr->type() != FOT_END ) {
            p.setPen( Qt::black);
            p.drawLine( x, y - CONNECT_HEIGHT, x, y);
        }
        curr = curr->next();
    }
}

void Column::drawCode( QPainter& p, int lineheight, int x, int& y)
{
    if ( m_root )
        m_root->drawCode( p, lineheight, x, y);
}

void Column::exportCode( QTextStream& out)
{
    if ( m_root )
        m_root->exportCode( out);
}
