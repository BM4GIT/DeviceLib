#include "language.h"
#include "flowcheck.h"

#define DIAMOND_WIDTH   40
#define DIAMOND_HEIGHT  78
#define END_WIDTH       20
#define END_HEIGHT      10
#define COLUMN_DIST     20   // distance between left and right column

FlowCheck::FlowCheck( void* column, FlowIt* fi) : FlowObject( column)
{
    m_fot = FOT_CHECK;
    m_colL.setMain( fi);
    m_colL.setParent( (Column*) column);
    m_colR.setMain( fi);
    m_colR.setParent( (Column*) column);
}

void FlowCheck::read( QTextStream& in)
{
    bool left = true;
    QString ln, tag, val;
    while ( !in.atEnd() ) {
        ln = in.readLine();
        g_parseTag( ln, tag, val);
        if ( tag == "<type>" ) m_fot = val.toInt();
        if ( tag == "<chart>" ) m_chart = val;
        if ( tag == "<code>" ) m_code = val;
        if ( tag == "<column>" ) {
            if ( left ) {
                m_colL.read( in);
                left = false;
            }
            else
                m_colR.read( in);
        }
        if ( tag == "</flowcheck>" ) break;
    }
}

void FlowCheck::write( QTextStream& out)
{
    FlowObject::write( out);
    m_colL.write( out);
    m_colR.write( out);
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << g_tabs + "</flowcheck>\n";
}

int FlowCheck::leftWidth()
{
    int w, h;
    calcSize( m_chart, w, h);
    if ( w < 100 ) w = 100;
    w += DIAMOND_WIDTH;
    if ( m_colL.leftWidth() > w )
        w = m_colL.leftWidth();
    return w;
}

int FlowCheck::width()
{
    int w, h;
    calcSize( m_chart, w, h);
    if ( w < 100 ) w = 100;
    w += DIAMOND_WIDTH;
    if ( m_colL.width() > w )
        w = m_colL.width();
    w += m_colR.width() + COLUMN_DIST;
    return w;
}

int FlowCheck::height()
{
    int w, h;
    calcSize( m_chart, w, h);
    h += DIAMOND_HEIGHT + CONNECT_HEIGHT + END_HEIGHT;
    int hL = m_colL.height();
    int hR = m_colR.height();
    h += (hL > hR ? hL : hR);
    return h;
}

Column* FlowCheck::columnLeft()
{
    return &m_colL;
}

Column* FlowCheck::columnRight()
{
    return &m_colR;
}

void FlowCheck::drawChart( QPainter& p)
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
    wL = m_colL.width();
    wR = m_colR.leftWidth();

    // draw condition diamond
    if ( inObject( mouseX, mouseY) && !g_dragging ) {
        g_fo = this;
        g_dotType = DT_OBJECT;
        p.setPen( Qt::cyan);
    }
    else
        p.setPen( Qt::black);
    p.drawLine( m_xpos, m_ypos, m_xpos - wD/2, m_ypos + hD/2);
    p.drawLine( m_xpos, m_ypos, m_xpos + wD/2, m_ypos + hD/2);
    p.drawLine( m_xpos - wD/2, m_ypos + hD/2, m_xpos, m_ypos + hD);
    p.drawLine( m_xpos + wD/2, m_ypos + hD/2, m_xpos, m_ypos + hD);

    // draw true/false
    p.setPen( Qt::black);
    p.setFont( QFont( "Arial", 8));
    p.drawText( QRect( m_xpos + wD/2 - 10, m_ypos + hD/2 - 20, 20, 10), Qt::AlignCenter, FO_NO);
    p.drawText( QRect( m_xpos + 5, m_ypos + hD - 5, 20, 10), Qt::AlignCenter, FO_YES);

    // draw condition
    p.setPen( Qt::blue);
    p.setFont( QFont( "Arial", 12));
    p.drawText( QRect( m_xpos - wD/2, m_ypos + 10, wD, hD/2), Qt::AlignCenter, FO_CHECK);
    p.setPen( Qt::blue);
    p.drawText( QRect( m_xpos - wD/2, m_ypos + 10, wD, hD), Qt::AlignCenter, m_chart);

    yL = yR = m_ypos + hD + CONNECT_HEIGHT;

    // connect left column
    p.setPen( Qt::black);
    p.drawLine( m_xpos , m_ypos + hD, m_xpos, yL);
    m_colL.drawChart( p, m_xpos, yL);

    // draw left connection circle
    if ( inConnectLeft( mouseX, mouseY) && (g_foDrag || g_owDrag) ) {
        g_fo = this;
        g_dotType = DT_LEFT;
        p.setPen( Qt::green);
    }
    else
        p.setPen( Qt::black);
    p.drawEllipse( m_xpos - 2, m_ypos + m_height + 4, 4, 4);

    // connect right column
    if ( wL < wD )
        xR = m_xpos + wD/2 + wR/2 + COLUMN_DIST;
    else
        xR = m_xpos + wL - m_colL.leftWidth()/2 + wR/2 + COLUMN_DIST;
    p.setPen( Qt::black);
    p.drawLine( m_xpos + wD/2, m_ypos + hD/2, xR, m_ypos + hD/2);
    p.drawLine( xR, m_ypos + hD/2, xR, yR);
    m_colR.drawChart( p, xR, yR);

    // draw right connection circle
    if ( inConnectRight( mouseX, mouseY) && (g_foDrag || g_owDrag) ) {
        g_fo = this;
        g_dotType = DT_RIGHT;
        p.setPen( Qt::green);
    }
    else
        p.setPen( Qt::black);
    p.drawEllipse( m_xpos + wD/2 + 4, m_ypos + hD/2 - 2, 4, 4);

    // draw left and right column to equal size
    p.setPen( Qt::black);
    if ( yR > yL ) {
        p.drawLine( m_xpos, yL, m_xpos, yR);
        yL = yR;
    }
    else {
        p.drawLine( xR, yR, xR, yL);
        yR = yL;
    }

    // connect right column to merge diamond
    p.setPen( Qt::black);
    p.drawLine( xR, yR, xR, yR + END_HEIGHT/2);
    p.drawLine( xR, yR + END_HEIGHT/2, m_xpos + END_WIDTH/2, yR + END_HEIGHT/2);

    // draw merge diamond
    p.setPen( Qt::black);
    p.drawLine( m_xpos, yL, m_xpos + END_WIDTH/2, yL + END_HEIGHT/2);
    p.drawLine( m_xpos + END_WIDTH/2, yL + END_HEIGHT/2, m_xpos, yL + END_HEIGHT);
    p.drawLine( m_xpos, yL + END_HEIGHT, m_xpos - END_WIDTH/2, yL + END_HEIGHT/2);
    p.drawLine( m_xpos - END_WIDTH/2, yL + END_HEIGHT/2, m_xpos, yL);

    // paint connection circle
    if ( inConnect( mouseX, mouseY) && (g_foDrag || g_owDrag) ) {
        g_fo = this;
        g_dotType = DT_CONNECT;
        p.setPen( Qt::green);
    }
    else
        p.setPen( Qt::black);
    p.drawEllipse( m_xpos - 2, yL + END_HEIGHT + 4, 4, 4);
}

void FlowCheck::drawCode( QPainter& p, int lineheight, int x, int& y)
{
    QString txt;
    makeCode( txt);
    p.drawText( QRect( x, y, 1000, lineheight), "if ( " + txt + " ) {");
    y += lineheight;
    if ( m_colL.root() )
        m_colL.root()->drawCode( p, lineheight, x + 30, y);
    p.drawText( QRect( x, y, 1000, lineheight), "}");
    y += lineheight;
    if ( m_colR.root() ) {
        p.drawText( QRect( x, y, 1000, lineheight), "else {");
        y += lineheight;
        m_colR.root()->drawCode( p, lineheight, x + 30, y);
        p.drawText( QRect( x, y, 1000, lineheight), "}");
        y += lineheight;
    }
    if ( m_next )
        m_next->drawCode( p, lineheight, x, y);
}

void FlowCheck::exportCode( QTextStream& out)
{
    QString txt;
    makeCode( txt);
    if ( !txt.isEmpty() ) {
        out << g_tabs + "if ( " + txt + " ) {\n";
        if ( m_colL.root() ) {
            g_tabs += '\t';
            m_colL.root()->exportCode( out);
            g_tabs = g_tabs.left( g_tabs.length() - 1);
        }
        out << g_tabs + "}\n";
        if ( m_colR.root() ) {
            out << g_tabs + "else {\n";
            g_tabs += '\t';
            m_colR.root()->exportCode( out);
            g_tabs = g_tabs.left( g_tabs.length() - 1);
            out << g_tabs + "}\n";
        }
    }
    if ( m_next )
        m_next->exportCode( out);
}

bool FlowCheck::inConnect( int x, int y)
{
    int yp = m_ypos + height();
    return x > m_xpos - 10 &&
           x < m_xpos + 10 &&
           y > yp &&
           y < yp + CONNECT_HEIGHT;
}

bool FlowCheck::inConnectLeft( int x, int y)
{
    int yp = m_ypos + m_height;
    return x > m_xpos - 10 &&
           x < m_xpos + 10 &&
           y > yp &&
           y < yp + CONNECT_HEIGHT;
}

bool FlowCheck::inConnectRight( int x, int y)
{
    int xp = m_xpos + m_width/2;
    int yp = m_ypos + m_height/2;
    return x > xp &&
           x < xp + COLUMN_DIST &&
           y > yp - 10 &&
           y < yp + 10;
}

bool FlowCheck::hasInstance( QString name)
{
    if ( m_code.indexOf( name + ".") >= 0 ) return true;
    if ( m_code.indexOf( name + "`") >= 0 ) return true;
    if ( m_colL.hasInstance( name) ) return true;
    if ( m_colR.hasInstance( name) ) return true;
    return FlowObject::hasInstance( name);
}

void FlowCheck::destroy()
{
    FlowObject* colL = m_colL.root();
    FlowObject* colR = m_colR.root();
    if ( colL ) {
        colL->destroy();
        delete colL;
        colL = NULL;
    }
    if ( colR ) {
        colR->destroy();
        delete colR;
        colR = NULL;
    }
    FlowObject::destroy();
}
