#ifndef FLOWWAIT_H
#define FLOWWAIT_H

#include "flowit.h"

class FlowWait : public FlowObject
{
public:
    FlowWait( void* column);

    void read( QTextStream& in);
    void write( QTextStream& out);

    int width();
    int height();

    QString chartText();
    int getValue();

    void drawChart( QPainter& p);
    void drawCode( QPainter& p, int lineheight, int x, int& y);
    void exportCode( QTextStream& out);
};

#endif // FLOWWAIT_H
