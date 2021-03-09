#include "language.h"
#include "flowsensor.h"

FlowSensor::FlowSensor( void* column) :
    FlowObject( column)
{
    m_fot = FOT_SENSOR;
    m_templ = NULL;
}

void FlowSensor::init( QString name, QString instance, Template* templ)
{
    m_name = name;
    m_inst = instance;
    m_templ = templ;
}

void FlowSensor::read( QTextStream& in)
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
        if ( tag == "</flowsensor>" ) break;
    }

    if ( m_templ ) m_templ->init( m_fot, func);
}

void FlowSensor::write( QTextStream& out)
{
    FlowObject::write( out);
    out << g_tabs + "<name>" << m_name << "</name>\n";
    out << g_tabs + "<include>" << m_templ->getInclude() << "</include>\n";
    out << g_tabs + "<instance>" << m_inst << "</instance>\n";
    g_tabs = g_tabs.left( g_tabs.length() - 1);
    out << g_tabs + "</flowsensor>\n";
}

QString FlowSensor::getInstance()
{
    return m_inst;
}

Template* FlowSensor::getTemplate()
{
    return m_templ;
}

bool FlowSensor::hasInstance( QString name)
{
    if ( m_inst == name ) return true;
    return FlowObject::hasInstance( name);
}

int FlowSensor::width()
{
    int w, h;
    calcSize( m_chart, w, h);
    return w + 40;
}

int FlowSensor::height()
{
    int w, h;
    calcSize( m_chart, w, h);
    return h + 20;
}

void FlowSensor::drawChart( QPainter& p)
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
    p.drawLine( x + w - 20, m_ypos, x + w - 20, m_ypos + h);
    p.drawLine( x + w - 20, m_ypos, x + w, m_ypos + h/2);
    p.drawLine( x + w - 20, m_ypos + h, x + w, m_ypos + h/2);
    p.setPen( Qt::black);
    p.setFont( QFont( "Arial", 8));
    p.drawText( QRect( x + 5, m_ypos, w - 5, h), Qt::AlignLeft, m_name);
    p.setPen( Qt::blue);
    p.setFont( QFont( "Arial", 12));
    p.drawText( QRect( x, m_ypos + 5, w - 20, h - 5), Qt::AlignCenter, m_chart);

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
