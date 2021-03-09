#include "language.h"
#include "flowdo.h"

FlowDo::FlowDo( void* column) : FlowObject( column)
{
    m_fot = FOT_DO;
}

void FlowDo::read( QTextStream& in)
{
    QString ln, tag, val;
    while ( !in.atEnd() ) {
        ln = in.readLine();
        g_parseTag( ln, tag, val);
        if ( tag == "<type>" ) m_fot = val.toInt();
        if ( tag == "<chart>" ) m_chart = val;
        if ( tag == "<code>" ) m_code = val;
        if ( tag == "</flowdo>" ) break;
    }
}

void FlowDo::write( QTextStream& out)
{
    FlowObject::write( out);
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << g_tabs + "</flowdo>\n";
}

int FlowDo::width()
{
    int w, h;
    calcSize( m_chart, w, h);
    return w + 10;
}

int FlowDo::height()
{
    int w, h;
    calcSize( m_chart, w, h);
    return h + 10;
}

void FlowDo::drawChart( QPainter& p)
{
    int w = width();
    int h = height();
    int x = m_xpos - w/2;
    m_width = w;
    m_height = h;

    // paint object
    if ( inObject( mouseX, mouseY) && !g_dragging ) {
        g_fo = this;
        g_dotType = DT_OBJECT;
        p.setPen( Qt::cyan);
    }
    else
        p.setPen( Qt::black);
    p.drawArc( x, m_ypos + 1, h, h - 1, 16*90, 16*180);
    p.drawArc( x + w - h, m_ypos + 1, h, h, 16*270, 16*180);
    p.drawLine( x + h/2, m_ypos, x + w - h/2, m_ypos);
    p.drawLine( x + h/2, m_ypos + h, x + w - h/2, m_ypos + h);
    p.setPen( Qt::blue);
    p.setFont( QFont( "Arial", 12));
    p.drawText( QRect( x, m_ypos, w, h), Qt::AlignCenter, m_chart);

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
