#include "language.h"
#include "flowactuator.h"

FlowActuator::FlowActuator( void* column) :
     FlowObject( column)
{
    m_fot = FOT_ACTUATOR;
    m_templ = NULL;
}

void FlowActuator::init( QString name, QString instance, Template* templ)
{
    m_name = name;
    // class: use the instance itself
    // module: use the name as alias for the instance
    m_inst = (instance.isEmpty() ? name : instance);
    m_templ = templ;
}

void FlowActuator::read( QTextStream& in)
{
    QString ln, tag, val;
    QStringList func;
    m_templ = NULL;
    while ( !in.atEnd() ) {
        ln = in.readLine();
        g_parseTag( ln, tag, val);
        if ( tag == "<type>" ) m_fot = val.toInt();
        if ( tag == "<chart>" ) m_chart = val;
        if ( tag == "<code>" ) m_code = val;
        if ( tag == "<name>" ) m_name = val;
        if ( tag == "<instance>" ) m_inst = val;
        if ( tag == "<function>" ) func.append( val);
        if ( tag == "<include>" ) m_templ = g_findTemplate( val);
        if ( tag == "</flowactuator>" ) break;
    }

    if ( m_templ ) m_templ->init( m_fot, func);
}

void FlowActuator::write( QTextStream& out)
{
    FlowObject::write( out);
    out << g_tabs + "<name>" << m_name << "</name>\n";
    out << g_tabs + "<include>" << m_templ->getInclude() << "</include>\n";
    out << g_tabs + "<instance>" << m_inst << "</instance>\n";
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << g_tabs + "</flowactuator>\n";
}

QString FlowActuator::getInstance()
{
    return m_inst;
}

Template* FlowActuator::getTemplate()
{
    return m_templ;
}

bool FlowActuator::hasInstance( QString name)
{
    if ( m_inst == name ) return true;
    return FlowObject::hasInstance( name);
}

int FlowActuator::width()
{
    int w, h;
    calcSize( m_chart, w, h);
    return w + 40;
}

int FlowActuator::height()
{
    int w, h;
    calcSize( m_chart, w, h);
    return h + 20;
}

void FlowActuator::drawChart( QPainter& p)
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
    p.drawRect( x, m_ypos, w, h);
    p.drawLine( x + 20, m_ypos, x + 20, m_ypos + h);
    p.drawLine( x, m_ypos, x + 20, m_ypos + h/2);
    p.drawLine( x, m_ypos + h, x + 20, m_ypos + h/2);
    p.setPen( Qt::black);
    p.setFont( QFont( "Arial", 8));
    p.drawText( QRect( x + 25, m_ypos, w - 25, h), Qt::AlignLeft, m_name);
    p.setPen( Qt::blue);
    p.setFont( QFont( "Arial", 12));
    p.drawText( QRect( x + 20, m_ypos + 5, w - 20, h - 5), Qt::AlignCenter, m_chart);

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
