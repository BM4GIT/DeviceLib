#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QString>

extern void setLanguage();

extern QString I_SUPPLYNAME;
extern QString I_DUPLICATENAME;
extern QString I_SUPPLYINCLUDE;
extern QString I_SUPPLYCLASS;
extern QString I_SUPPLYDECLARATION;
extern QString I_NOMOVEINSUB;
extern QString I_NODELSETUPORLOOP;
extern QString I_NODELPAGE;
extern QString I_DELROUTINE;
extern QString I_DELPAGE;
extern QString I_CODESUCCESS;
extern QString I_CODEFAILURE;
extern QString I_PDFSUCCESS;
extern QString I_PDFFAILURE;
extern QString I_PRINTSUCCESS;
extern QString I_PRINTFAILURE;
extern QString I_OBJECTINUSE;
extern QString I_INVALIDROUTINE;
extern QString I_INVALIDDECL;
extern QString I_NOPAGENESTING;

extern QString T_ASKACCEPT;
extern QString T_ASKREJECT;
extern QString T_ADDROUTINE;
extern QString T_DELROUTINE;
extern QString T_LBLROUTINE;
extern QString T_ADDVARIABLE;
extern QString T_DELVARIABLE;
extern QString T_LBLVARIABLE;
extern QString T_ADDACTUATOR;
extern QString T_ADDSENSOR;
extern QString T_DELDEVICE;
extern QString T_LBLDEVICE;
extern QString T_VARTYPE;
extern QString T_DEVCLASS;
extern QString T_DEVUSING;
extern QString T_DEVINCLUDE;
extern QString T_DEVDECLARE;
extern QString T_DECLDO;
extern QString T_DECLREJECT;
extern QString T_FILCHARTTEXT;
extern QString T_FILCODETEXT;
extern QString T_FILFUNCCALL;
extern QString T_FILGENERAL;
extern QString T_FILFUNCTION;
extern QString T_FILDECLARATION;
extern QString T_EXPTITLE;
extern QString T_EXPARDUINO;
extern QString T_EXPRASPBERRY;
extern QString T_EXPFLOWCHART;
extern QString T_EXPSAVEAS;
extern QString T_EXPPRINTCHART;
extern QString T_EXPPRINTER;
extern QString T_EXPDO;
extern QString T_EXPCANCEL;

extern QString T_NOTALLOWED;
extern QString T_OPENPROJECT;
extern QString T_EXPORT;

extern QString V_FIPFILES;
extern QString V_INOFILES;
extern QString V_CPPFILES;
extern QString V_PNGFILES;
extern QString V_OURAPP;
extern QString V_PAGE;

extern QString FO_CHECK;
extern QString FO_WHILE;
extern QString FO_REPEAT;
extern QString FO_PAGE;
extern QString FO_WAIT;
extern QString FO_DO;
extern QString FO_TIMES;
extern QString FO_MSEC;
extern QString FO_READY;
extern QString FO_YES;
extern QString FO_NO;

#endif // LANGUAGE_H
