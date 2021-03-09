#ifndef FLOWPAGE_H
#define FLOWPAGE_H

#include "flowit.h"

#define END_WIDTH       20  // end triangle
#define END_HEIGHT      15

class FlowPage : public FlowObject
{
public:
    FlowPage( void* column);

    void init( QString name);

    void read( QTextStream& in);
    void write( QTextStream& out);

    void setAction( QAction* action);
    QAction* getAction();

    QString name();

    int width();
    int height();

    void setNextPage( FlowPage* page);
    static void removePage( FlowPage* page);
    static int freePageNr();

    void drawChart( QPainter& p);
    void drawCode( QPainter& p, int lineheight, int x, int& y);
    void exportCode( QTextStream& out);

protected:
    FlowPage*   m_nextPage;
    QString     m_name;
    QAction*    m_action;
};

#endif // FLOWPAGE_H
