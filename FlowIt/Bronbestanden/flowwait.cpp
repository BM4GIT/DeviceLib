#include "language.h"
#include "flowwait.h"

FlowWait::FlowWait( void* column) : FlowObject( column)
{
    m_fot = FOT_WAIT;
}

void FlowWait::read( QTextStream& in)
{
    QString ln, tag, val;
    while ( !in.atEnd() ) {
        ln = in.readLine();
        g_parseTag( ln, tag, val);
        if ( tag == "<type>" ) m_fot = val.toInt();
        if ( tag == "<chart>" ) m_chart = val;
        if ( tag == "<code>" ) m_code = val;
        if ( tag == "</flowwait>" ) break;
    }
}

void FlowWait::write( QTextStream& out)
{
    FlowObject::write( out);
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << g_tabs + "</flowwait>\n";
}

int FlowWait::width()
{
    int w, h;
    calcSize( chartText(), w, h);
    return w + 10;
}

int FlowWait::height()
{
    int w, h;
    calcSize( chartText(), w, h);
    return h + 10;
}

QString FlowWait::chartText()
{
    if ( m_chart.isEmpty() )
        return FO_WAIT.toLower() + " ? " + FO_MSEC;
    else
        return FO_WAIT.toLower() + " " + m_chart + " " + FO_MSEC;
}

int FlowWait::getValue()
{
    return m_chart.toInt();
}

void FlowWait::drawChart( QPainter& p)
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
    p.drawText( QRect( x, m_ypos, w, h), Qt::AlignCenter, chartText());

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

void FlowWait::drawCode( QPainter& p, int lineheight, int x, int& y)
{
    QString txt = (m_code.isEmpty() ? "^?^" : m_code);
    p.drawText( QRect( x, y, 1000, lineheight), "delay( " + txt + ");");
    y += lineheight;
    if ( m_next )
        m_next->drawCode( p, lineheight, x, y);
}

void FlowWait::exportCode( QTextStream& out)
{
    QString txt = (m_code.isEmpty() ? "^?^" : m_code);
    out << g_tabs + "delay( " + txt + ");\n";
    if ( m_next )
        m_next->exportCode( out);
}
