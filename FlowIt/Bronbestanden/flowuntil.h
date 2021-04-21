#ifndef FLOWUNTIL_H
#define FLOWUNTIL_H

#include "flowit.h"

class FlowUntil : public FlowObject
{
public:
    FlowUntil( void* column, FlowIt* fi);

    void read( QTextStream& in);
    void write( QTextStream& out);

    int width();
    int leftWidth();
    int height();

    Column* column();

    void drawChart( QPainter& p);
    void drawCode( QPainter& p, int lineheight, int x, int& y);
    void exportCode( QTextStream& out);

    bool inObject( int x, int y);
    bool inConnect( int x, int y);
    bool inConnectLeft( int x, int y);

    bool hasInstance( QString name);

    void destroy();

protected:
    Column  m_colL;
};

#endif // FLOWUNTIL_H
