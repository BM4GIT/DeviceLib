#ifndef OBJECTWID_H
#define OBJECTWID_H

#include <QWidget>
#include "template.h"

#define DRAG_HOME   25

namespace Ui {
class ObjectWid;
}

class ObjectWid : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectWid( QWidget *parent, int fot = -1);
    ~ObjectWid();

    void init( int y, QString name);

    bool read( QTextStream& in);
    void write( QTextStream& out);

    void setDeclaration( QString classname, QString instance, QString include = "");
    int height();
    QString name();
    int type();
    QString getInstance();
    Template* getTemplate();
    bool inDrag( int x, int y);
    bool selected();
    void setHome( int ypos);
    void home();

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::ObjectWid*  ui;
    int             m_fot;
    QString         m_name;
    int             m_ypos;
    int             m_height;
    QString         m_inst;
    QString         m_incl;
    Template*       m_templ;
    bool            m_sel;
};

#endif // OBJECTWID_H
