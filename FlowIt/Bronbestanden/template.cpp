#include "language.h"
#include "flowit.h"
#include <QFile>
#include <QTextStream>

Template::Template()
{
    m_fot = -1;
}

void Template::init( int fot, QStringList functions)
{
    m_fot = fot;
    m_func.append( functions);
}

int Template::fot()
{
    return m_fot;
}

bool Template::read( QTextStream& in)
{
    QString ln, tag, val;
    while ( !in.atEnd() ) {
        ln = in.readLine();
        g_parseTag( ln, tag, val);
        if ( tag == "<type>" ) {
            if ( val.toLower() == "actuator" )
                m_fot = FOT_ACTUATOR;
            if ( val.toLower() == "sensor" )
                m_fot = FOT_SENSOR;
            if ( val.toLower() == "storage" )
                m_fot = FOT_STORAGE;
            if ( val.toLower() == "interface" )
                m_fot = FOT_INTERFACE;
        }
        if ( tag == "<include>" ) m_incl = val;
        if ( tag == "<using>" ) m_using = val;
        if ( tag == "<class>" ) m_class = val;
        if ( tag == "<module>" ) m_module = val;
        if ( tag == "<function>" ) m_func.append( val);
        if ( tag == "</template>" ) break;
    }
    if ( m_incl.isEmpty() || m_class.isEmpty() )
        return false;
    return (g_findTemplate( m_incl) ? false : true);
}

void Template::write( QTextStream& out)
{
    out << "<template>\n";

    switch ( m_fot ) {
        case FOT_ACTUATOR:  out << "\t<type>actuator</type>\n"; break;
        case FOT_SENSOR:    out << "\t<type>sensor</type>\n"; break;
        case FOT_STORAGE:   out << "\t<type>storage</type>\n"; break;
        case FOT_INTERFACE: out << "\t<type>interface</type>\n"; break;
    }

    if ( !m_incl.isEmpty() )
        out << "\t<include>" << m_incl << "</include>\n";
    if ( !m_using.isEmpty() )
        out << "\t<using>" << m_using << "</using>\n";
    if ( !m_class.isEmpty() )
        out << "\t<class>" << m_class << "</class>\n";
    if ( !m_module.isEmpty() )
        out << "\t<module>" << m_module << "</module>\n";
    for ( int i = 0; i < m_func.size(); i++ )
        out << "\t<function>" << m_func.at( i) << "</function>\n";

    out << "</template>\n\n";
}

void Template::import( QString filepath)
{
    QFile file( filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in( &file);
    QStringList line;
    QString     ln,entry;
    int         ix;

    read( in);

    file.close();
}

QString Template::getInclude()
{
    return m_incl;
}

QString Template::getUsing()
{
    return m_using;
}

QString Template::getClass()
{
    return m_class;
}

QString Template::getModule()
{
    return m_module;
}

QString Template::getFunction( int ix)
{
    QString func;
    if ( ix >= 0 && ix < m_func.size() )
        func = m_func.at( ix);
    return func;
}

int Template::functionCount()
{
    return m_func.size();
}
