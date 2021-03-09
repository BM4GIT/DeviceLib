#include <QFile>
#include <QTextStream>

QString I_SUPPLYNAME;
QString I_DUPLICATENAME;
QString I_SUPPLYINCLUDE;
QString I_SUPPLYCLASS;
QString I_SUPPLYDECLARATION;
QString I_NOMOVEINSUB;
QString I_NODELSETUPORLOOP;
QString I_NODELPAGE;
QString I_DELROUTINE;
QString I_DELPAGE;
QString I_CODESUCCESS;
QString I_CODEFAILURE;
QString I_PDFSUCCESS;
QString I_PDFFAILURE;
QString I_PRINTSUCCESS;
QString I_PRINTFAILURE;
QString I_OBJECTINUSE;
QString I_INVALIDROUTINE;
QString I_INVALIDDECL;
QString I_NOPAGENESTING;

QString T_ASKACCEPT;
QString T_ASKREJECT;
QString T_ADDROUTINE;
QString T_DELROUTINE;
QString T_LBLROUTINE;
QString T_ADDVARIABLE;
QString T_DELVARIABLE;
QString T_LBLVARIABLE;
QString T_ADDACTUATOR;
QString T_ADDSENSOR;
QString T_DELDEVICE;
QString T_LBLDEVICE;
QString T_VARTYPE;
QString T_DEVCLASS;
QString T_DEVUSING;
QString T_DEVINCLUDE;
QString T_DEVDECLARE;
QString T_DECLDO;
QString T_DECLREJECT;
QString T_FILCHARTTEXT;
QString T_FILCODETEXT;
QString T_FILFUNCCALL;
QString T_FILGENERAL;
QString T_FILFUNCTION;
QString T_FILDECLARATION;
QString T_EXPTITLE;
QString T_EXPARDUINO;
QString T_EXPRASPBERRY;
QString T_EXPFLOWCHART;
QString T_EXPSAVEAS;
QString T_EXPPRINTCHART;
QString T_EXPPRINTER;
QString T_EXPDO;
QString T_EXPCANCEL;

QString T_NOTALLOWED;
QString T_OPENPROJECT;
QString T_EXPORT;

QString V_FIPFILES;
QString V_INOFILES;
QString V_CPPFILES;
QString V_PNGFILES;
QString V_OURAPP;
QString V_PAGE;
QString V_LONG;
QString V_LONGLIST;
QString V_FLOAT;
QString V_FLOATLIST;
QString V_STRING;
QString V_STRINGLIST;
QString V_BOOL;

QString FO_CHECK;
QString FO_WHILE;
QString FO_REPEAT;
QString FO_PAGE;
QString FO_WAIT;
QString FO_DO;
QString FO_TIMES;
QString FO_MSEC;
QString FO_READY;
QString FO_YES;
QString FO_NO;

void setLanguage()
{
    QFile file( "language.fi");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in( &file);

    QString ln, tag, val;
    int ix;

    while ( !in.atEnd() ) {
        ln = in.readLine().trimmed();
        if ( (ix = ln.indexOf( '=')) < 0 ) continue;
        tag = ln.left( ix).trimmed();
        val = ln.right( ln.length() - ix - 1).trimmed();

        if ( tag == "I_SUPPLYNAME" ) I_SUPPLYNAME = val; else
        if ( tag == "I_DUPLICATENAME" ) I_DUPLICATENAME = val; else
        if ( tag == "I_SUPPLYINCLUDE" ) I_SUPPLYINCLUDE = val; else
        if ( tag == "I_SUPPLYCLASS" ) I_SUPPLYCLASS = val; else
        if ( tag == "I_SUPPLYDECLARATION" ) I_SUPPLYDECLARATION = val; else
        if ( tag == "I_NOMOVEINSUB" ) I_NOMOVEINSUB = val; else
        if ( tag == "I_NODELSETUPORLOOP" ) I_NODELSETUPORLOOP = val; else
        if ( tag == "I_NODELPAGE" ) I_NODELPAGE = val; else
        if ( tag == "I_DELROUTINE" ) I_DELROUTINE = val; else
        if ( tag == "I_DELPAGE" ) I_DELPAGE = val; else
        if ( tag == "I_CODESUCCESS" ) I_CODESUCCESS = val; else
        if ( tag == "I_CODEFAILURE" ) I_CODEFAILURE = val; else
        if ( tag == "I_PDFSUCCESS" ) I_PDFSUCCESS = val; else
        if ( tag == "I_PDFFAILURE" ) I_PDFFAILURE = val; else
        if ( tag == "I_PRINTSUCCESS" ) I_PRINTSUCCESS = val; else
        if ( tag == "I_PRINTFAILURE" ) I_PRINTFAILURE = val; else
        if ( tag == "I_OBJECTINUSE" ) I_OBJECTINUSE = val; else
        if ( tag == "I_INVALIDROUTINE" ) I_INVALIDROUTINE = val; else
        if ( tag == "I_INVALIDDECL" ) I_INVALIDDECL = val; else
        if ( tag == "I_NOPAGENESTING" ) I_NOPAGENESTING = val; else

        if ( tag == "T_ASKACCEPT" ) T_ASKACCEPT = val; else
        if ( tag == "T_ASKREJECT" ) T_ASKREJECT = val; else
        if ( tag == "T_ADDROUTINE" ) T_ADDROUTINE = val; else
        if ( tag == "T_DELROUTINE" ) T_DELROUTINE = val; else
        if ( tag == "T_LBLROUTINE" ) T_LBLROUTINE = val; else
        if ( tag == "T_ADDVARIABLE" ) T_ADDVARIABLE = val; else
        if ( tag == "T_DELVARIABLE" ) T_DELVARIABLE = val; else
        if ( tag == "T_LBLVARIABLE" ) T_LBLVARIABLE = val; else
        if ( tag == "T_ADDACTUATOR" ) T_ADDACTUATOR = val; else
        if ( tag == "T_ADDSENSOR" ) T_ADDSENSOR = val; else
        if ( tag == "T_DELDEVICE" ) T_DELDEVICE = val; else
        if ( tag == "T_LBLDEVICE" ) T_LBLDEVICE = val; else
        if ( tag == "T_VARTYPE" ) T_VARTYPE = val; else
        if ( tag == "T_DEVCLASS" ) T_DEVCLASS = val; else
        if ( tag == "T_DEVUSING" ) T_DEVUSING = val; else
        if ( tag == "T_DEVINCLUDE" ) T_DEVINCLUDE = val; else
        if ( tag == "T_DEVDECLARE" ) T_DEVDECLARE = val; else
        if ( tag == "T_DECLDO" ) T_DECLDO = val; else
        if ( tag == "T_DECLREJECT" ) T_DECLREJECT = val; else
        if ( tag == "T_FILCHARTTEXT" ) T_FILCHARTTEXT = val; else
        if ( tag == "T_FILCODETEXT" ) T_FILCODETEXT = val; else
        if ( tag == "T_FILFUNCCALL" ) T_FILFUNCCALL = val; else
        if ( tag == "T_FILGENERAL" ) T_FILGENERAL = val; else
        if ( tag == "T_FILFUNCTION" ) T_FILFUNCTION = val; else
        if ( tag == "T_FILDECLARATION" ) T_FILDECLARATION = val; else
        if ( tag == "T_EXPTITLE" ) T_EXPTITLE = val; else
        if ( tag == "T_EXPARDUINO" ) T_EXPARDUINO = val; else
        if ( tag == "T_EXPRASPBERRY" ) T_EXPRASPBERRY = val; else
        if ( tag == "T_EXPFLOWCHART" ) T_EXPFLOWCHART = val; else
        if ( tag == "T_EXPSAVEAS" ) T_EXPSAVEAS = val; else
        if ( tag == "T_EXPPRINTCHART" ) T_EXPPRINTCHART = val; else
        if ( tag == "T_EXPPRINTER" ) T_EXPPRINTER = val; else
        if ( tag == "T_EXPDO" ) T_EXPDO = val; else
        if ( tag == "T_EXPCANCEL" ) T_EXPCANCEL = val; else
        if ( tag == "T_NOTALLOWED" ) T_NOTALLOWED = val; else
        if ( tag == "T_OPENPROJECT" ) T_OPENPROJECT = val; else
        if ( tag == "T_EXPORT" ) T_EXPORT = val; else

        if ( tag == "V_FIPFILES" ) V_FIPFILES = val; else
        if ( tag == "V_INOFILES" ) V_INOFILES = val; else
        if ( tag == "V_CPPFILES" ) V_CPPFILES = val; else
        if ( tag == "V_PNGFILES" ) V_PNGFILES = val; else
        if ( tag == "V_OURAPP" ) V_OURAPP = val; else
        if ( tag == "V_PAGE" ) V_PAGE = val; else

        if ( tag == "FO_CHECK" ) FO_CHECK = val; else
        if ( tag == "FO_WHILE" ) FO_WHILE = val; else
        if ( tag == "FO_REPEAT" ) FO_REPEAT = val; else
        if ( tag == "FO_PAGE" ) FO_PAGE = val; else
        if ( tag == "FO_WAIT" ) FO_WAIT = val; else
        if ( tag == "FO_DO" ) FO_DO = val; else
        if ( tag == "FO_TIMES" ) FO_TIMES = val; else
        if ( tag == "FO_MSEC" ) FO_MSEC = val; else
        if ( tag == "FO_READY" ) FO_READY = val; else
        if ( tag == "FO_YES" ) FO_YES = val; else
        if ( tag == "FO_NO" ) FO_NO = val; else
        if ( tag == "V_PAGE" ) V_PAGE = val;
    }

    file.close();
}
