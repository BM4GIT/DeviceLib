#include "language.h"
#include "flowit.h"
#include <QImage>

int mouseX = 0;
int mouseY = 0;

////////////////
// FLOWOBJECT //
////////////////

FlowObject::FlowObject( void* column)
{
    m_column = column;
    m_fot = -1;
    m_next = m_prev = NULL;
    m_xpos = m_ypos = m_width = m_height = 0;
}

void FlowObject::write( QTextStream& out)
{
    switch ( m_fot ) {
    case FOT_ACTUATOR:  out << g_tabs + "<flowactuator>\n"; break;
    case FOT_CHECK:     out << g_tabs + "<flowcheck>\n"; break;
    case FOT_WHILE:     out << g_tabs + "<flowwhile>\n"; break;
    case FOT_REPEAT:    out << g_tabs + "<flowrepeat>\n"; break;
    case FOT_PAGE:      out << g_tabs + "<flowpage>\n"; break;
    case FOT_WAIT:      out << g_tabs + "<flowwait>\n"; break;
    case FOT_DO:        out << g_tabs + "<flowdo>\n"; break;
    case FOT_ROUTINE:   out << g_tabs + "<flowroutine>\n"; break;
    case FOT_SENSOR:    out << g_tabs + "<flowsensor>\n"; break;
    }

    g_tabs += '\t';
    out << g_tabs + "<type>" << m_fot << "</type>\n";
    out << g_tabs + "<chart>" << m_chart << "</chart>\n";
    out << g_tabs + "<code>" << m_code << "</code>\n";
}

void FlowObject::setColumn(void *column)
{
    m_column = column;
}

void* FlowObject::column()
{
    return m_column;
}

int FlowObject::type()
{
    return m_fot;
}

void FlowObject::calcSize( QString text, int& width, int& height)
{
    if ( text.isEmpty() ) {
        width = 20;
        height = 20;
        return;
    }
    QRect rct( 0, 0, 300, 300);
    QImage img( 300, 300, QImage::Format_ARGB32);
    QPainter p( &img);
    p.setFont(QFont("Arial", 12));
    p.drawText( rct, Qt::AlignCenter, text, &rct);
    width = rct.width();
    height = rct.height();
}

void FlowObject::setPosition( int x, int y)
{
    m_xpos = x;
    m_ypos = y;
}

void FlowObject::setChartText( QString text)
{
    m_chart = text;
}

void FlowObject::setCodeText( QString text)
{
    m_code = text;
}

QString FlowObject::chartText()
{
    return m_chart;
}

QString FlowObject::codeText()
{
    return m_code;
}

bool FlowObject::inObject( int x, int y)
{
    return x > m_xpos - m_width/2 &&
           x < m_xpos + m_width/2 &&
           y > m_ypos &&
           y < m_ypos + m_height;
}

bool FlowObject::inConnect( int x, int y)
{
    return x > m_xpos - 10 &&
           x < m_xpos + 10 &&
           y > m_ypos + m_height &&
           y < m_ypos + m_height + CONNECT_HEIGHT;
}

bool FlowObject::hasInstance( QString name)
{
    if ( m_next ) return m_next->hasInstance( name);
    return false;
}

bool FlowObject::hasVariable( QString name)
{
    QString an = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    QString cd = m_code;
    int ix, len = name.length();
    while ( (ix = cd.indexOf( name)) >= 0 ){
        if ( (ix > 0 && an.indexOf( cd[ix-1]) >= 0) ||
             (ix+len < cd.length() && an.indexOf( cd[ix+len]) >= 0) ) {
            cd = cd.mid( ix+len);
        }
        else
            return true;
    }
    if ( m_next ) return m_next->hasVariable( name);
    return false;
}

FlowObject* FlowObject::first()
{
    FlowObject* curr = this;
    while ( curr->m_prev )
        curr = curr->m_prev;
    return curr;
}

FlowObject* FlowObject::prev()
{
    return m_prev;
}

FlowObject* FlowObject::next()
{
    return m_next;
}

FlowObject* FlowObject::last()
{
    FlowObject* curr = this;
    while ( curr->m_next )
        curr = curr->m_next;
    return curr;
}

void FlowObject::attachBegin( FlowObject* fo)
// positions 'fo' at the begin of the linked list
{
    FlowObject* curr = first();
    fo->m_prev = NULL;
    fo->m_next = curr;
    if ( curr )
        curr->m_prev = fo;
    ((Column*)m_column)->setRoot( fo);
    fo->m_column = m_column;
}

void FlowObject::attachBefore( FlowObject* fo)
// positions 'fo' before 'this'
{
    if ( m_prev )
        m_prev->m_next = fo;
    fo->m_prev = m_prev;
    fo->m_next = this;
    m_prev = fo;
    if ( !fo->prev() )
        ((Column*)m_column)->setRoot( fo);
    fo->m_column = m_column;
}

void FlowObject::attachAfter( FlowObject* fo)
// positions 'fo' after 'this'
{
    if ( m_next )
        m_next->m_prev = fo;
    fo->m_next = m_next;
    fo->m_prev = this;
    m_next = fo;
    fo->m_column = m_column;
}

void FlowObject::attachEnd( FlowObject* fo)
// positions 'fo' at the end of the linked list
{
    FlowObject* curr = last();
    fo->m_next = NULL;
    fo->m_prev = curr;
    if ( curr )
        curr->m_next = fo;
    fo->m_column = m_column;
}

void FlowObject::detach()
{
    if ( m_prev )
        m_prev->m_next = m_next;
    else
        ((Column*) m_column)->setRoot( m_next);
    if ( m_next )
        m_next->m_prev = m_prev;
    m_prev = NULL;
    m_next = NULL;
    m_column = NULL;
}

void FlowObject::destroy()
{
    if ( m_next ) {
        m_next->destroy();
        delete m_next;
        m_next = NULL;
    }
}

void FlowObject::drawCode( QPainter& p, int lineheight, int x, int& y)
{
    QString txt;
    makeCode( txt);
    if ( !txt.isEmpty() ) {
        p.drawText( QRect( x, y, 1000, lineheight), txt + ";");
        y += lineheight;
    }
    if ( m_next )
        m_next->drawCode( p, lineheight, x, y);
}

void FlowObject::exportCode( QTextStream& out)
{
    QString txt;
    makeCode( txt);
    if ( !txt.isEmpty() )
        out << g_tabs + txt + ";\n";
    if ( m_next )
        m_next->exportCode( out);
}

void FlowObject::makeCode( QString& txt)
{
    txt = m_code.trimmed();
    while ( txt.right( 1) == ";" )
        txt = txt.left( txt.length() - 1);
    if ( txt.isEmpty() ) {
        txt = m_chart.trimmed();
        if ( !txt.isEmpty() )
            txt = "^ " + txt + " ^";
    }
}

/////////////
// FLOWEND //
/////////////

FlowEnd::FlowEnd( void* column) : FlowObject( column)
{
    m_fot = FOT_END;
}

void FlowEnd::read( QTextStream& in)
{
    QString ln, tag, val;
    while ( !in.atEnd() ) {
        ln = in.readLine();
        g_parseTag( ln, tag, val);
        if ( tag == "<type>" ) m_fot = val.toInt();
        if ( tag == "<chart>" ) m_chart = val;
        if ( tag == "<code>" ) m_code = val;
        if ( tag == "</flowend>" ) break;
    }
}

void FlowEnd::write( QTextStream& out)
{
}

int FlowEnd::width()
{
    return 50;
}

int FlowEnd::height()
{
    return 30;
}

void FlowEnd::drawChart( QPainter& p)
{
    int w = width();
    int h = height();
    int x = m_xpos;
    int y = m_ypos;
    int dx = x - w/2;

    if ( ((Column*) m_column)->root()->type() == FOT_ROUTINE ) {
        QBrush br( Qt::gray);
        p.setPen( QPen( br, 3));
        p.drawEllipse( dx, y, w, h);
        p.setPen( Qt::black);
        p.drawEllipse( dx, y, w - 1, h - 1);
        p.setPen(Qt::blue);
        p.setFont(QFont("Arial", 12));
        p.drawText( QRect( dx, y, w, h), Qt::AlignCenter, "eind");
    }
    else {
        p.drawLine( m_xpos - END_WIDTH/2, m_ypos, m_xpos + END_WIDTH/2, m_ypos);
        p.drawLine( m_xpos - END_WIDTH/2, m_ypos, m_xpos, m_ypos + END_HEIGHT);
        p.drawLine( m_xpos + END_WIDTH/2, m_ypos, m_xpos, m_ypos + END_HEIGHT);
        p.setPen( Qt::blue);
        p.setFont( QFont( "Arial", 8));
        p.drawText( QRect( m_xpos - END_WIDTH/2 + 1, m_ypos, END_WIDTH, END_HEIGHT - 3), Qt::AlignCenter, m_chart);
    }
}
