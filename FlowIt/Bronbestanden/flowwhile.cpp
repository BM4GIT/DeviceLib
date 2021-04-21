#include "language.h"
#include "flowwhile.h"

#define DIAMOND_WIDTH   40  // main diamond
#define DIAMOND_HEIGHT  78
#define END_WIDTH       20  // end triangle
#define END_HEIGHT      7
#define RIGHT_WIDTH     40  // right connection column

FlowWhile::FlowWhile( void* column, FlowIt* fi) : FlowObject( column)
{
    m_fot = FOT_WHILE;
    m_colL.setMain( fi);
    m_colL.setParent( (Column*) column);
}

void FlowWhile::read( QTextStream& in)
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
        if ( tag == "</flowwhile>" ) break;
    }
}

void FlowWhile::write( QTextStream& out)
{
    FlowObject::write( out);
    m_colL.write( out);
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << g_tabs + "</flowwhile>\n";
}

int FlowWhile::leftWidth()
{
    int w, h;
    calcSize( m_chart, w, h);
    if ( w < 100 ) w = 100;
    w += DIAMOND_WIDTH;
    if ( m_colL.leftWidth() > w )
        w = m_colL.leftWidth();
    return w;
}

int FlowWhile::width()
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

int FlowWhile::height()
{
    int w, h;
    calcSize( m_chart, w, h);
    h += DIAMOND_HEIGHT + END_HEIGHT +2*CONNECT_HEIGHT;
    return h + m_colL.height();
}

Column* FlowWhile::column()
{
    return &m_colL;
}

void FlowWhile::drawChart( QPainter& p)
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
    p.drawText( QRect( m_xpos + wD/2 - 10, m_ypos + hD/2 - 20, 30, 10), Qt::AlignCenter, FO_READY);
    p.drawText( QRect( m_xpos + 5, m_ypos + hD - 5, 20, 14), Qt::AlignCenter, FO_YES);

    // draw condition
    p.setPen( Qt::blue);
    p.setFont( QFont( "Arial", 12));
    p.drawText( QRect( m_xpos - wD/2 + 3, m_ypos + 10, wD, hD/2), Qt::AlignCenter, FO_WHILE);
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
    if ( wL < wD/2 )
        xR = m_xpos + wD/2 + wR/2;
    else
        xR = m_xpos + wL + wR/2;
    p.setPen( Qt::black);
    p.drawLine( m_xpos + wD/2, m_ypos + hD/2, xR, m_ypos + hD/2);
    p.drawLine( xR, m_ypos + hD/2, xR, yR);

    // draw left and right column to equal size
    if ( yR > yL ) {
        p.drawLine( m_xpos, m_ypos, m_xpos, yR);
        yL = yR;
    }
    else {
        p.drawLine( xR, yR, xR, yL);
        yR = yL;
    }

    // draw repeat ellipse
    p.setPen( Qt::black);
    p.drawLine( m_xpos, yL, m_xpos - END_WIDTH/2, yL + END_HEIGHT);
    p.drawLine( m_xpos, yL, m_xpos + END_WIDTH/2, yL + END_HEIGHT);
    p.drawLine( m_xpos - END_WIDTH/2, yL + END_HEIGHT, m_xpos + END_WIDTH/2, yL + END_HEIGHT);

    // connect right column to left column
    p.setPen( Qt::black);
    p.drawLine( xR, yR, xR, yR + END_HEIGHT + CONNECT_HEIGHT);
    p.drawLine( xR, yR + END_HEIGHT + CONNECT_HEIGHT, m_xpos, yR + END_HEIGHT + CONNECT_HEIGHT);

    // paint connection circle
    if ( inConnect( mouseX, mouseY) && (g_foDrag || g_owDrag) ) {
        g_fo = this;
        g_dotType = DT_CONNECT;
        p.setPen( Qt::green);
    }
    else
        p.setPen( Qt::black);
    yL += END_HEIGHT + CONNECT_HEIGHT;
    p.drawEllipse( m_xpos - 2, yL + 4, 4, 4);
}

void FlowWhile::drawCode( QPainter& p, int lineheight, int x, int& y)
{
    QString txt;
    makeCode( txt);
    p.drawText( QRect( x, y, 1000, lineheight), "while ( " + txt + " ) {");
    y += lineheight;
    if ( m_colL.root() )
        m_colL.root()->drawCode( p, lineheight, x + 30, y);
    p.drawText( QRect( x, y, 1000, lineheight), "}");
    y += lineheight;
    if ( m_next )
        m_next->drawCode( p, lineheight, x, y);
}

void FlowWhile::exportCode( QTextStream& out)
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

bool FlowWhile::inConnect( int x, int y)
{
    int yp = m_ypos + height();
    return x > m_xpos - 10 &&
           x < m_xpos + 10 &&
           y > yp &&
           y < yp + CONNECT_HEIGHT;
}

bool FlowWhile::inConnectLeft( int x, int y)
{
    int yp = m_ypos + m_height;
    return x > m_xpos - 10 &&
           x < m_xpos + 10 &&
           y > yp &&
           y < yp + CONNECT_HEIGHT;
}

bool FlowWhile::hasInstance( QString name)
{
    if ( m_code.indexOf( name + ".") >= 0 ) return true;
    if ( m_code.indexOf( name + "`") >= 0 ) return true;
    if ( m_colL.hasInstance( name) ) return true;
    return FlowObject::hasInstance( name);
}

void FlowWhile::destroy()
{
    FlowObject* root = m_colL.root();
    if ( root ) {
        root->destroy();
        delete root;
        root = NULL;
    }
    FlowObject::destroy();
}
