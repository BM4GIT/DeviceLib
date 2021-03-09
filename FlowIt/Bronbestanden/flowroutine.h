#ifndef FLOWROUTINE_H
#define FLOWROUTINE_H

#include "flowobject.h"

class FlowRoutine : public FlowObject
{
public:
    FlowRoutine( void* column);

    void init( QString name);

    void read( QTextStream& in);
    void write( QTextStream& out);

    void setAction( QAction* action);
    QAction* getAction();

    QString name();

    int width();
    int height();

    void drawChart( QPainter& p);
    void drawCode( QPainter& p, int lineheight, int x, int& y);
    void exportCode( QTextStream& out);

protected:
    QString     m_name;
    QAction*    m_action;
};

#endif // FLOWROUTINE_H
