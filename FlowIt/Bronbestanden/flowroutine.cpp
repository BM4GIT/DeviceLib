#include "language.h"
#include "flowroutine.h"
#include "column.h"
#include "flowit.h"

FlowRoutine::FlowRoutine( void* column) : FlowObject( column)
{
    m_fot = FOT_ROUTINE;
    ((Column*)column)->setRoot( this);
    ((Column*)column)->setParent( NULL);
}

void FlowRoutine::init( QString name)
{
    m_name = name;
}

void FlowRoutine::read( QTextStream& in)
{
    QString ln, tag, val;
    while ( !in.atEnd() ) {
        ln = in.readLine();
        g_parseTag( ln, tag, val);
        if ( tag == "<type>" ) m_fot = val.toInt();
        if ( tag == "<chart>" ) m_chart = val;
        if ( tag == "<code>" ) m_code = val;
        if ( tag == "<name>" ) m_name = val;
        if ( tag == "</flowroutine>" ) break;
    }
}

void FlowRoutine::write( QTextStream& out)
{
    FlowObject::write( out);
    out << g_tabs + "<name>" << m_name << "</name>\n";
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << g_tabs + "</flowroutine>\n";
}

void FlowRoutine::setAction( QAction* action)
{
    m_action = action;
}

QAction* FlowRoutine::getAction()
{
    return m_action;
}

QString FlowRoutine::name()
{
    return m_name;
}

int FlowRoutine::width()
{
    int w, h;
    calcSize( m_name, w, h);
    return w + 20;
}

int FlowRoutine::height()
{
    int w, h;
    calcSize( m_name, w, h);
    return h + 10;
}

void FlowRoutine::drawChart( QPainter& p)
{
    int w = width();
    int h = height();
    int x = m_xpos - w/2;
    m_width = w;
    m_height = h;

    QBrush br( Qt::gray);
    p.setPen( QPen( br, 3));
    p.drawEllipse( x, m_ypos, w, h);

    // draw ellipse
    if ( inObject( mouseX, mouseY) && !g_dragging ) {
        g_foTool = this;
        p.setPen( Qt::cyan);
    }
    else
        p.setPen( Qt::black);
    p.drawEllipse( x, m_ypos, w - 1, h - 1);
    p.setPen( Qt::blue);
    p.setFont( QFont( "Arial", 12));
    p.drawText( QRect( x, m_ypos, w, h), Qt::AlignCenter, m_name);

    // paint connection
    if ( inConnect( mouseX, mouseY) && (g_foDrag || g_owDrag) ) {
        g_fo = this;
        g_dotType = DT_CONNECT;
        p.setPen( Qt::green);
    }
    else
        p.setPen( Qt::black);
    p.drawEllipse( m_xpos - 2, m_ypos + h + 4, 4, 4);
}

void FlowRoutine::drawCode( QPainter& p, int lineheight, int x, int& y)
{
    p.drawText( QRect( x, y, 1000, lineheight), "void " + m_name + "()");
    y += lineheight;
    p.drawText( QRect( x, y, 1000, lineheight), "{");
    y += lineheight;
    x += 30;
    m_next->drawCode( p, lineheight, x, y);
    x -= 30;
    p.drawText( QRect( x, y, 1000, lineheight), "}");
    y += 2*lineheight;
}

void FlowRoutine::exportCode( QTextStream& out)
{
    out << "void " + m_name + "()\n{\n";
    g_tabs += '\t';
    m_next->exportCode( out);
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << "}\n\n";
}
