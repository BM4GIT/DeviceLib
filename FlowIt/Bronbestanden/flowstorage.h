#ifndef FLOWSTORAGE_H
#define FLOWSTORAGE_H

#include "flowit.h"

class FlowStorage : public FlowObject
{
public:
    FlowStorage( void* column);

    void init( QString name, QString instance, Template* templ);

    void read( QTextStream& in);
    void write( QTextStream& out);

    QString getInstance();
    Template* getTemplate();

    bool hasInstance( QString name);

    int width();
    int height();

    void drawChart( QPainter& p);

protected:
    QString     m_name;
    QString     m_inst;
    Template*   m_templ;
};

#endif // FLOWSTORAGE_H
