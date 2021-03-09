#include "language.h"
#include "objectwid.h"
#include "flowit.h"
#include "ui_objectwid.h"
#include <QPainter>

#define OW_WIDTH    100
#define OW_HEIGHT   30

ObjectWid::ObjectWid( QWidget *parent, int fot) :
    QWidget(parent),
    ui(new Ui::ObjectWid)
{
    ui->setupUi(this);
    setWindowModality( Qt::WindowModal);
    m_fot = fot;
    m_ypos = 0;
    m_height = 0;
    m_templ = NULL;
}

ObjectWid::~ObjectWid()
{
    delete ui;
}

void ObjectWid::init( int y, QString name)
{
    m_ypos = y;
    m_name = name;

    switch ( m_fot ) {
        case FOT_CHECK:
        case FOT_WHILE:
        case FOT_REPEAT:    m_height = OW_HEIGHT * 2; break;
        default:            m_height = OW_HEIGHT;
    }
}

bool ObjectWid::read( QTextStream& in)
{
    QString ln;
    while ( !in.atEnd() ) {
        QString tag, val;
        ln = in.readLine();
        g_parseTag( ln, tag, val);
        if ( tag == "<type>" ) m_fot = val.toInt();
        if ( tag == "<name>" ) m_name = val;
        if ( tag == "<include>" ) {
            m_incl = val;
            m_templ = g_findTemplate( m_incl);
        }
        if ( tag == "<instance>" ) m_inst = val;
        if ( tag == "</declaration>" ) break;
    }
}

void ObjectWid::write( QTextStream& out)
{
    out << "<declaration>\n";
    out << "\t<type>" << m_fot << "</type>\n";
    out << "\t<name>" << m_name << "</name>\n";
    out << "\t<include>" << m_incl << "</include>\n";
    out << "\t<instance>" << m_inst << "</instance>\n";
    out << "</declaration>\n\n";
}

void ObjectWid::setDeclaration( QString classname, QString instance, QString include)
{
    m_templ = g_findTemplate( include);
    m_incl = include;
    m_inst = instance;
}

int ObjectWid::height()
{
    return m_height;
}

QString ObjectWid::name()
{
    return m_name;
}

int ObjectWid::type()
{
    return m_fot;
}

Template* ObjectWid::getTemplate()
{
    return m_templ;
}

QString ObjectWid::getInstance()
{
    return m_inst;
}

bool ObjectWid::inDrag( int x, int y)
{
    int yp = m_ypos + height() / 2;
    return x > DRAG_HOME + 100 &&
           x < DRAG_HOME + 110 &&
           y > yp - 5 &&
           y < yp + 5;
}

bool ObjectWid::selected()
{
    return m_sel;
}

void ObjectWid::setHome( int ypos)
{
    m_ypos = ypos;
}

void ObjectWid::home()
{
    move( DRAG_HOME, m_ypos);
}

void ObjectWid::paintEvent(QPaintEvent *)
{
    m_sel = ((m_fot == FOT_ACTUATOR || m_fot == FOT_SENSOR) && underMouse());

    // it is advised to use QImage as an intermediate
    // painting device in order to be sure to get identical
    // results on different real devices

    QImage img( 2000, 1500, QImage::Format_ARGB32);
    QPainter p( &img);
    p.setPen( m_sel ? Qt::cyan : Qt::darkGreen);
    p.setFont( QFont( "Arial", 12));

    switch ( m_fot ) {
    case FOT_CHECK:         p.drawLine( 0, m_height/2, OW_WIDTH/2, 0);
                            p.drawLine( OW_WIDTH/2, 0, OW_WIDTH, m_height/2);
                            p.drawLine( OW_WIDTH, m_height/2, OW_WIDTH/2, m_height);
                            p.drawLine( OW_WIDTH/2, m_height, 0, m_height/2);
                            p.setPen( Qt::darkGreen);
                            p.drawText( QRect( 0, 0, OW_WIDTH, m_height), Qt::AlignCenter, FO_CHECK);
                            resize( OW_WIDTH+1, m_height+1);
                            break;

    case FOT_WHILE:         p.drawLine( 0, m_height/2, OW_WIDTH/2, 0);
                            p.drawLine( OW_WIDTH/2, 0, OW_WIDTH, m_height/2);
                            p.drawLine( OW_WIDTH, m_height/2, OW_WIDTH/2, m_height);
                            p.drawLine( OW_WIDTH/2, m_height, 0, m_height/2);
                            p.setPen( Qt::darkGreen);
                            p.drawText( QRect( 0, 0, OW_WIDTH, m_height), Qt::AlignCenter, FO_WHILE);
                            resize( OW_WIDTH+1, m_height+1);
                            break;

    case FOT_REPEAT:        p.drawLine( 0, m_height/2, OW_WIDTH/2, 0);
                            p.drawLine( OW_WIDTH/2, 0, OW_WIDTH, m_height/2);
                            p.drawLine( OW_WIDTH, m_height/2, OW_WIDTH/2, m_height);
                            p.drawLine( OW_WIDTH/2, m_height, 0, m_height/2);
                            p.setPen( Qt::darkGreen);
                            p.drawText( QRect( 0, 0, OW_WIDTH, m_height), Qt::AlignCenter, FO_REPEAT);
                            resize( OW_WIDTH+1, m_height+1);
                            break;

    case FOT_PAGE:          p.drawLine( OW_WIDTH/2 - 10, m_height/2, OW_WIDTH/2 + 10, m_height/2);
                            p.drawLine( OW_WIDTH/2 + 10, m_height/2, OW_WIDTH/2, m_height);
                            p.drawLine( OW_WIDTH/2, m_height, OW_WIDTH/2 - 10, m_height/2);
                            p.setPen( Qt::darkGreen);
                            p.setFont( QFont( "Arial", 10));
                            p.drawText( QRect( 0, 0, OW_WIDTH, m_height/2 - 5), Qt::AlignCenter, FO_PAGE);
                            resize( OW_WIDTH+1, m_height+1);
                            break;

    case FOT_WAIT:          p.drawArc( 0, 1, m_height, m_height-1, 16*(OW_WIDTH-10), 16*180);
                            p.drawArc( OW_WIDTH - m_height, 1, m_height, m_height, 16*270, 16*180);
                            p.drawLine( m_height/2, 0, OW_WIDTH-m_height/2, 0);
                            p.drawLine( m_height/2, m_height, OW_WIDTH-m_height/2, m_height);
                            p.setPen( Qt::darkGreen);
                            p.drawText( QRect( 0, 0, OW_WIDTH, m_height), Qt::AlignCenter, FO_WAIT);
                            resize( OW_WIDTH+1, m_height+1);
                            break;

    case FOT_DO:            p.drawArc( 0, 1, m_height, m_height-1, 16*(OW_WIDTH-10), 16*180);
                            p.drawArc( OW_WIDTH - m_height, 1, m_height, m_height, 16*270, 16*180);
                            p.drawLine( m_height/2, 0, OW_WIDTH-m_height/2, 0);
                            p.drawLine( m_height/2, m_height, OW_WIDTH-m_height/2, m_height);
                            p.setPen( Qt::darkGreen);
                            p.drawText( QRect( 0, 0, OW_WIDTH, m_height), Qt::AlignCenter, FO_DO);
                            resize( OW_WIDTH+1, m_height+1);
                            break;

    case FOT_ACTUATOR:      p.drawRect( 0, 0, OW_WIDTH, m_height);
                            p.drawLine( 10, 0, 10, m_height);
                            p.drawLine( 0, 0, 10, m_height/2);
                            p.drawLine( 0, m_height, 10, m_height/2);
                            p.setPen( Qt::darkGreen);
                            p.drawText( QRect( 10, 5, OW_WIDTH, m_height-5), Qt::AlignCenter, m_name);
                            p.setFont( QFont( "Arial", 8));
                            p.drawText( QRect( 15, 0, OW_WIDTH, m_height), Qt::AlignLeft, m_inst);
                            resize( OW_WIDTH+1, m_height+1);
                            break;

    case FOT_SENSOR:        p.drawRect( 0, 0, OW_WIDTH, m_height);
                            p.drawLine( OW_WIDTH-10, 0, OW_WIDTH-10, m_height);
                            p.drawLine( OW_WIDTH-10, 0, OW_WIDTH, m_height/2);
                            p.drawLine( OW_WIDTH-10, m_height, OW_WIDTH, m_height/2);
                            p.setPen( Qt::darkGreen);
                            p.drawText( QRect( 0, 5, OW_WIDTH-10, m_height-5), Qt::AlignCenter, m_name);
                            p.setFont( QFont( "Arial", 8));
                            p.drawText( QRect( 5, 0, OW_WIDTH-10, m_height), Qt::AlignLeft, m_inst);
                            resize( OW_WIDTH+1, m_height+1);
                            break;

    default:                break;
    }


    QPainter pw( this);
    pw.drawImage( 0, 0, img);
}
