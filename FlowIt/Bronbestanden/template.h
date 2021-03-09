#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QStringList>
#include <QTextStream>

class Template
{
public:

    Template();

    void init( int fot, QStringList functions);

    int fot();

    bool read( QTextStream& in);
    void write( QTextStream& out);

    void import( QString filepath);

    QString getInclude();
    QString getUsing();
    QString getClass();
    QString getFunction( int ix);
    int     functionCount();

protected:

    int     m_fot;
    QString m_incl;
    QString m_using;
    QString m_class;
    QStringList m_func;
};

#endif // TEMPLATE_H
