#include "flowit.h"

FlowPage::FlowPage( void* column) :
    FlowObject( column)
{
    m_fot = FOT_PAGE;
    m_nextPage = NULL;
}

void FlowPage::init( QString name)
{
    m_name = name;
}

void FlowPage::read( QTextStream& in)
{
    QString ln, tag, val;
    while ( !in.atEnd() ) {
        ln = in.readLine();
        g_parseTag( ln, tag, val);
        if ( tag == "<type>" ) m_fot = val.toInt();
        if ( tag == "<chart>" ) m_chart = val;
        if ( tag == "<code>" ) m_code = val;
        if ( tag == "<name>" ) m_name = val;
        if ( tag == "</flowpage>" ) break;
    }
}

void FlowPage::write( QTextStream& out)
{
    FlowObject::write( out);
    out << g_tabs + "<name>" << m_name << "</name>\n";
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << g_tabs + "</flowpage>\n";
}

void FlowPage::setAction( QAction* action)
{
    m_action = action;
}

QAction* FlowPage::getAction()
{
    return m_action;
}

QString FlowPage::name()
{
    return m_name;
}

int FlowPage::width()
{
    return END_WIDTH;
}

int FlowPage::height()
{
    return END_HEIGHT;
}

void FlowPage::setNextPage( FlowPage* page)
{
    m_nextPage = page;
}

void FlowPage::removePage( FlowPage* page)
{
    FlowPage* curr = g_rootPage;
    FlowPage* prev = NULL;
    while ( curr && (curr != page) ) {
        prev = curr;
        curr = curr->m_nextPage;
    }
    if ( curr == page )
    {
        if ( prev )
            prev->m_nextPage = curr->m_nextPage;
        else // this is the root page
            g_rootPage = g_rootPage->m_nextPage;
    }
}

int FlowPage::freePageNr()
{
    if ( !g_rootPage ) return 1;
    FlowPage* curr;
    int n = 1;
    while ( true ) {
        curr = g_rootPage;
        while ( curr ) {
            if ( curr->chartText().toInt() == n )
                break;
            curr = curr->m_nextPage;
        }
        if ( !curr ) return n;
        n++;
    }
    return 1;
}

void FlowPage::drawChart( QPainter& p)
{
    m_width = END_WIDTH;
    m_height = END_HEIGHT;

    // paint object
    if ( inObject( mouseX, mouseY) && !g_dragging ) {
        if ( m_name.isEmpty() )
            g_fo = this;
        else
            g_foTool = this;
        g_dotType = DT_OBJECT;
        p.setPen( Qt::cyan);
    }
    else
        p.setPen( Qt::black);

    p.drawLine( m_xpos - END_WIDTH/2, m_ypos, m_xpos + END_WIDTH/2, m_ypos);
    p.drawLine( m_xpos - END_WIDTH/2, m_ypos, m_xpos, m_ypos + END_HEIGHT);
    p.drawLine( m_xpos + END_WIDTH/2, m_ypos, m_xpos, m_ypos + END_HEIGHT);
    p.setPen( Qt::blue);
    p.setFont( QFont( "Arial", 8));
    p.drawText( QRect( m_xpos - END_WIDTH/2 + 1, m_ypos, END_WIDTH, END_HEIGHT - 3), Qt::AlignCenter, m_chart);

    // paint connection
    if ( inConnect( mouseX, mouseY) && (g_foDrag || g_owDrag) ) {
        g_fo = this;
        g_dotType = DT_CONNECT;
        p.setPen( Qt::green);
    }
    else
        p.setPen( Qt::black);
    p.drawEllipse( m_xpos - 2, m_ypos + END_HEIGHT + 4, 4, 4);
}

void FlowPage::drawCode( QPainter& p, int lineheight, int x, int& y)
{
    if ( m_name.isEmpty() ) { // than it's a page link
        for ( int i = 0; i < g_tools.size(); i++ )
            if ( g_tools.at( i)->root()->chartText() == m_chart ) {
                g_tools.at( i)->drawCode( p, lineheight, x, y);
                break;
            }
    }
    if ( m_next )
        m_next->drawCode( p, lineheight, x, y);
}

void FlowPage::exportCode( QTextStream& out)
{
    if ( m_name.isEmpty() ) { // than it's a page link
        for ( int i = 0; i < g_tools.size(); i++ )
            if ( g_tools.at( i)->root()->chartText() == m_chart ) {
                g_tools.at( i)->exportCode( out);
                break;
            }
    }
    if ( m_next )
        m_next->exportCode( out);
}
