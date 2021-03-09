#ifndef FLOWOBJECT_H
#define FLOWOBJECT_H

#include <QPainter>
#include <QTextStream>
#include <QAction>

#define FOT_ROUTINE     0
#define FOT_END         1
#define FOT_CHECK       2
#define FOT_WHILE       3
#define FOT_REPEAT      4
#define FOT_WAIT        5
#define FOT_DO          6
#define FOT_ACTUATOR    7
#define FOT_SENSOR      8
#define FOT_PAGE        9

extern int mouseX;
extern int mouseY;

#define CONNECT_HEIGHT  20

class FlowObject
{
public:
    FlowObject( void* column);

    virtual void read( QTextStream& in) {}
    virtual void write( QTextStream& out);

    void setColumn( void* column);
    void* column();

    virtual void setAction( QAction* action) {}
    virtual QAction* getAction() { return NULL; }

    virtual QString name() { return ""; }
    int type();

    static void calcSize( QString text, int& width, int& height);
    void setPosition( int x, int y);

    void setChartText( QString text); // comes from FilloutDlg
    void setCodeText( QString text);  // comes from FilloutDlg

    QString chartText();
    QString codeText();
    virtual int getValue() {}

    virtual int width() { return 0; }
    virtual int leftWidth() { return width(); }
    virtual int height() { return 0; }

    virtual void drawChart( QPainter& p) {}
    virtual void drawCode( QPainter& p, int lineheight, int x, int& y);
    virtual void exportCode( QTextStream& out);

    bool inObject( int x, int y);
    virtual bool inConnect( int x, int y);
    virtual bool inConnectRight( int x, int y) { return false; }

    virtual bool hasInstance( QString name);
    virtual bool hasVariable( QString name);

    // COLUMN HANDLING

    FlowObject* first();
    FlowObject* prev();
    FlowObject* next();
    FlowObject* last();

    void attachBegin( FlowObject* fo);
    void attachBefore( FlowObject* fo);
    void attachAfter( FlowObject* fo);
    void attachEnd( FlowObject* fo);

    void detach();
    virtual void destroy();
    // Warning: destroy will delete all objects attached
    // after this one. If not desired: detach before destroy

protected:
    void makeCode( QString& txt);

    void*       m_column;

    int         m_fot;

    int         m_xpos;
    int         m_ypos;
    int         m_width;
    int         m_height;

    QString     m_chart; // chart text, comes from FilloutDlg
    QString     m_code;  // code text, comes from FilloutDlg

    FlowObject* m_prev;
    FlowObject* m_next;
};

class FlowEnd : public FlowObject
{
public:
    FlowEnd( void* column);

    void read( QTextStream& in);
    void write( QTextStream& out);

    int width();
    int height();

    void drawChart( QPainter& p);
    void drawCode( QPainter& p, int lineheight, int x, int& y) {}
    void exportCode( QTextStream &out) {}

protected:
    int m_type;
};

#endif // FLOWOBJECT_H
