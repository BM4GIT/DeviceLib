#include "language.h"
#include "flowuntil.h"

#define DIAMOND_WIDTH   40  // main diamond
#define DIAMOND_HEIGHT  78
#define END_WIDTH       20  // end triangle
#define END_HEIGHT      10
#define RIGHT_WIDTH     40  // right connection column

FlowUntil::FlowUntil( void* column, FlowIt* fi) : FlowObject( column)
{
    m_fot = FOT_UNTIL;
    m_colL.setMain( fi);
    m_colL.setParent( (Column*) column);
}

void FlowUntil::read( QTextStream& in)
{
    QString ln, tag, val;
    while ( !in.atEnd() ) {
        ln = in.readLine();
        g_parseTag( ln, tag, val);
        if ( tag == "<type>" ) m_fot = val.toInt();
        if ( tag == "<chart>" ) m_chart = val;
        if ( tag == "<code>" ) m_code = val;
        if ( tag == "<column>" )
            m_colL.read( in);
        if ( tag == "</flowuntil>" ) break;
    }
}

void FlowUntil::write( QTextStream& out)
{
    FlowObject::write( out);
    m_colL.write( out);
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << g_tabs + "</flowuntil>\n";
}

int FlowUntil::leftWidth()
{
    int w, h;
    calcSize( m_chart, w, h);
    if ( w < 100 ) w = 100;
    w += DIAMOND_WIDTH;
    if ( m_colL.leftWidth() > w )
        w = m_colL.leftWidth();
    return w;
}

int FlowUntil::width()
{
    int w, h;
    calcSize( m_chart, w, h);
    if ( w < 100 ) w = 100;
    w += DIAMOND_WIDTH;
    if ( m_colL.width() > w )
        w = m_colL.width();
    w += RIGHT_WIDTH;
    return w;
}

int FlowUntil::height()
{
    int w, h;
    calcSize( m_chart, w, h);
    h += DIAMOND_HEIGHT + END_HEIGHT + CONNECT_HEIGHT;
    return h + m_colL.height();
}

Column* FlowUntil::column()
{
    return &m_colL;
}

void FlowUntil::drawChart( QPainter& p)
{
    int wD, hD, wL, wR, yL, xR, yR;

    // diamond size
    calcSize( m_chart, wD, hD);
    if ( wD < 100 ) wD = 100;
    wD += DIAMOND_WIDTH;
    hD += DIAMOND_HEIGHT;
    m_width = wD;
    m_height = hD;

    // column widths
    wL = m_colL.width() - m_colL.leftWidth()/2;
    wR = RIGHT_WIDTH;

    // draw merge diamond
    p.setPen( Qt::black);
    p.drawLine( m_xpos, m_ypos, m_xpos + END_WIDTH/2, m_ypos + END_HEIGHT/2);
    p.drawLine( m_xpos, m_ypos, m_xpos - END_WIDTH/2, m_ypos + END_HEIGHT/2);
    p.drawLine( m_xpos + END_WIDTH/2, m_ypos + END_HEIGHT/2, m_xpos, m_ypos + END_HEIGHT);
    p.drawLine( m_xpos - END_WIDTH/2, m_ypos + END_HEIGHT/2, m_xpos, m_ypos + END_HEIGHT);

    yL = yR = m_ypos + END_HEIGHT;

    // connect left column
    p.setPen( Qt::black);
    p.drawLine( m_xpos , yL, m_xpos, yL + CONNECT_HEIGHT);

    // draw left connection circle
    if ( inConnectLeft( mouseX, mouseY) && (g_foDrag || g_owDrag) ) {
        g_fo = this;
        g_dotType = DT_LEFT;
        p.setPen( Qt::green);
    }
    else
        p.setPen( Qt::black);
    p.drawEllipse( m_xpos - 2, yL + 4, 4, 4);

    yL += CONNECT_HEIGHT;
    m_colL.drawChart( p, m_xpos, yL);

    // draw condition diamond
    if ( inObject( mouseX, mouseY) && !g_dragging ) {
        g_fo = this;
        g_dotType = DT_OBJECT;
        p.setPen( Qt::cyan);
    }
    else
        p.setPen( Qt::black);
    p.drawLine( m_xpos, yL, m_xpos - wD/2, yL + hD/2);
    p.drawLine( m_xpos, yL, m_xpos + wD/2, yL + hD/2);
    p.drawLine( m_xpos - wD/2, yL + hD/2, m_xpos, yL + hD);
    p.drawLine( m_xpos + wD/2, yL + hD/2, m_xpos, yL + hD);

    // draw true/false
    p.setPen( Qt::black);
    p.setFont( QFont( "Arial", 8));
    p.drawText( QRect( m_xpos + wD/2 - 10, yL + hD/2 - 20, 20, 10), Qt::AlignCenter, FO_NO);
    p.drawText( QRect( m_xpos + 5, yL + hD - 5, 30, 10), Qt::AlignCenter, FO_READY);

    // draw condition
    p.setPen( Qt::blue);
    p.setFont( QFont( "Arial", 12));
    p.drawText( QRect( m_xpos - wD/2 + 3, yL + 10, wD, hD/2), Qt::AlignCenter, FO_UNTIL);
    p.setPen( Qt::blue);
    p.drawText( QRect( m_xpos - wD/2, yL + 10, wD, hD), Qt::AlignCenter, m_chart);

    yL += hD;

    // paint connection circle
    if ( inConnect( mouseX, mouseY) && (g_foDrag || g_owDrag) ) {
        g_fo = this;
        g_dotType = DT_CONNECT;
        p.setPen( Qt::green);
    }
    else
        p.setPen( Qt::black);
    p.drawEllipse( m_xpos - 2, yL + 4, 4, 4);

    // connect right column to start diamond
    if ( wL < wD/2 )
        xR = m_xpos + wD/2 + wR/2;
    else
        xR = m_xpos + wL + wR/2;
    p.setPen( Qt::black);
    p.drawLine( m_xpos + END_WIDTH/2, m_ypos + END_HEIGHT/2, xR, m_ypos + END_HEIGHT/2);

    // draw return arrow
    int x = m_xpos + END_WIDTH/2 + 10;
    int y = m_ypos + END_HEIGHT/2;
    int dx = END_WIDTH/2;
    int dy = END_HEIGHT/2;
    p.drawLine( x, y, x + dx, y - dy);
    p.drawLine( x, y, x + dx, y + dy);

    // draw left and right column to equal size
    p.drawLine( xR, m_ypos + END_HEIGHT/2, xR, yL - hD/2);

    // connect right column to condition diamond
    p.drawLine( m_xpos + wD/2, yL - hD/2, xR, yL - hD/2);
}

void FlowUntil::drawCode( QPainter& p, int lineheight, int x, int& y)
{
    QString txt;
    makeCode( txt);
    p.drawText( QRect( x, y, 1000, lineheight), "do {");
    y += lineheight;
    if ( m_colL.root() )
        m_colL.root()->drawCode( p, lineheight, x + 30, y);
    p.drawText( QRect( x, y, 1000, lineheight), "} while ( !(" + txt + ") );");
    y += lineheight;
    if ( m_next )
        m_next->drawCode( p, lineheight, x, y);
}

void FlowUntil::exportCode( QTextStream& out)
{
    QString txt;
    makeCode( txt);
    out << g_tabs + "while ( " + txt + " ) {\n";
    if ( m_colL.root() ) {
        g_tabs += '\t';
        m_colL.root()->exportCode( out);
        g_tabs = g_tabs.left( g_tabs.length() - 1);
    }
    out << g_tabs + "}\n";
    if ( m_next )
        m_next->exportCode( out);
}

bool FlowUntil::inObject( int x, int y)
{
    int h = END_HEIGHT + CONNECT_HEIGHT + m_colL.height();
    return x > m_xpos - m_width/2 &&
           x < m_xpos + m_width/2 &&
           y > m_ypos + h &&
           y < m_ypos + h + m_height;
}

bool FlowUntil::inConnect( int x, int y)
{
    int yp = m_ypos + height();
    return x > m_xpos - 10 &&
           x < m_xpos + 10 &&
           y > yp &&
           y < yp + CONNECT_HEIGHT;
}

bool FlowUntil::inConnectLeft( int x, int y)
{
    int yp = m_ypos + END_HEIGHT;
    return x > m_xpos - 10 &&
           x < m_xpos + 10 &&
           y > yp &&
           y < yp + CONNECT_HEIGHT;
}

bool FlowUntil::hasInstance( QString name)
{
    if ( m_code.indexOf( name + ".") >= 0 ) return true;
    if ( m_code.indexOf( name + "`") >= 0 ) return true;
    if ( m_colL.hasInstance( name) ) return true;
    return FlowObject::hasInstance( name);
}

void FlowUntil::destroy()
{
    FlowObject* root = m_colL.root();
    if ( root ) {
        root->destroy();
        delete root;
        root = NULL;
    }
    FlowObject::destroy();
}
