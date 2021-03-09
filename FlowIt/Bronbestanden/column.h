#ifndef COLUMN_H
#define COLUMN_H

#include <QTextStream>
#include <QPainter>
#include "flowobject.h"

class Column
{
public:
    Column();
    ~Column();

    void read( QTextStream& in);
    void write( QTextStream& out);

    void setMain( void* fi);
    void setParent(  Column* parent);
    bool isChildOf( Column* column);

    bool hasInstance( QString name);
    bool hasVariable( QString name);

    void setRoot( FlowObject* fo);
    void insertRoot( FlowObject* fo);
    FlowObject* root();

    int width();
    int leftWidth();
    int height();

    void drawChart( QPainter& p, int x, int& y);
    void drawCode( QPainter& p, int lineheight, int x, int& y);
    void exportCode( QTextStream& out);

protected:

    void* m_fi;
    Column* m_parent;
    FlowObject* m_root;
};

#endif // COLUMN_H
