#ifndef FLOWIT_H
#define FLOWIT_H

#include <QMainWindow>
#include <QAction>
#include <QList>
#include <QStringList>
#include "template.h"
#include "objectwid.h"
#include "column.h"
#include "flowobject.h"
#include "flowroutine.h"
#include "flowpage.h"

#define STANDARDOBJECTS 7

#define DT_OBJECT   0   // flow object selected
#define DT_CONNECT  1   // fo connect circle selected
#define DT_LEFT     2   // fo left column circle selected
#define DT_RIGHT    3   // fo right column circle selected

typedef struct {
    QString name;
    QString type;
} TVariable;

extern bool         g_dragging; // causes highlighting of objects
extern ObjectWid*   g_owDrag;   // != NULL causes highlighting of connect circles
extern FlowObject*  g_foDrag;   // != NULL causes highlighting of connect circles
extern FlowObject*  g_fo;       // remembers the flowobject which is dragged
extern int          g_dotType;  // connection circle type
extern int          g_panOwY;   // vertical pan of the declaration window

extern QList<Column*> g_tools;    // root flowobject has the name of the tool
extern FlowObject*    g_foTool;   // root flowobject of the current tool

extern QList<TVariable*>    g_variables;
extern QList<Template*>     g_templates;
extern QList<Template*>     g_libraries;
extern QList<ObjectWid*>    g_objects;

extern QString g_tabs;
extern FlowPage* g_rootPage;

extern QString              g_savePath;
extern QString              g_exportPath;

extern Template* g_findTemplate( QString include);
extern void g_parseTag( QString ln, QString& tag, QString& val);

extern bool g_validName( QString name);

namespace Ui {
class FlowIt;
}

class FlowIt : public QMainWindow
{
    Q_OBJECT

public:
    explicit FlowIt(QWidget *parent = 0);
    ~FlowIt();

    bool declarationExists( QString declaration);

    void createTool( FlowObject* foTool);
    bool deleteTool( QString name);

    void createVariable( QString type, QString name);
    bool deleteVariable( QString name);

    void createObject( ObjectWid* ow, QString name, bool isclass = true);
    bool deleteObject( QString name);
    void listObjects();

    bool hasInstance( QString name);
    bool hasVariable( QString name);

    void addLibrary( Template* templ);

    void newProject();
    void openProject();
    void saveProject();

    void setLatestProject();
    void getLatestProject();

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent * event);

private slots:

    void on_file_triggered();
    void on_export_triggered();
    void on_device_triggered();
    void on_pan_triggered();
    void on_routine_triggered();

private:
    Ui::FlowIt *ui;

    void clear();
    void paintChart( QPainter& painter, Column* column, int x, int y, bool print = false);
    void paintCode( QPainter& painter, int x, int y, int h);
    void exportCode( QTextStream& out);
    void exportMake( QTextStream& in, QTextStream& out, QString path, QString file);

    int                 m_yObj; // position for next object widget

    Column*             m_column;

    QAction*            m_panchart;
    QAction*            m_drawchart;
    QAction*            m_adddev;
};

#endif // FLOWIT_H
