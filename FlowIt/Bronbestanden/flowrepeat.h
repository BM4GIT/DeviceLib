#ifndef FLOWREPEAT_H
#define FLOWREPEAT_H

#include "flowit.h"

class FlowRepeat : public FlowObject
{
public:
    FlowRepeat( void* column, FlowIt* fi);

    void read( QTextStream& in);
    void write( QTextStream& out);

    int width();
    int leftWidth();
    int height();

    int getValue();
    QString chartText();

    Column* column();

    void drawChart( QPainter& p);
    void drawCode( QPainter& p, int lineheight, int x, int& y);
    void exportCode( QTextStream& out);

    bool inConnect( int x, int y);
    bool inConnectLeft( int x, int y);

    bool hasInstance( QString name);

    void destroy();

protected:
    Column  m_colL;
};

#endif // FLOWREPEAT_H
