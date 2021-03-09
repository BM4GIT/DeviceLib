#ifndef FLOWCHECK_H
#define FLOWCHECK_H

#include "flowit.h"

class FlowCheck : public FlowObject
{
public:
    FlowCheck( void* column, FlowIt* fi);

    void read( QTextStream& in);
    void write( QTextStream& out);

    int width();
    int leftWidth();
    int height();

    Column* columnLeft();
    Column* columnRight();

    void drawChart( QPainter& p);
    void drawCode( QPainter& p, int lineheight, int x, int& y);
    void exportCode( QTextStream& out);

    bool inConnect( int x, int y);
    bool inConnectLeft( int x, int y);
    bool inConnectRight( int x, int y);

    bool hasInstance( QString name);

    void destroy();

protected:
    Column  m_colL;
    Column  m_colR;
};

#endif // FLOWCHECK_H
