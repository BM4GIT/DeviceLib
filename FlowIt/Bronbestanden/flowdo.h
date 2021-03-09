#ifndef FLOWDO_H
#define FLOWDO_H

#include "flowit.h"

class FlowDo : public FlowObject
{
public:
    FlowDo( void* column);

    void read( QTextStream& in);
    void write( QTextStream& out);

    int width();
    int height();

    void drawChart( QPainter& p);
};

#endif // FLOWDO_H
