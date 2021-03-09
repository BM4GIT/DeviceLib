#ifndef DEVICEDLG_H
#define DEVICEDLG_H

#include <QDialog>
#include "flowit.h"

namespace Ui {
class DeviceDlg;
}

class DeviceDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceDlg(QWidget *parent = 0);
    ~DeviceDlg();

    void setFI( FlowIt* fi);

private slots:
    void on_pbCancel_clicked();
    void on_pbDo_clicked();
    void on_rbRoutine_clicked();
    void on_rbVariable_clicked();
    void on_rbActuator_clicked();
    void on_rbSensor_clicked();
    void on_rbRoutineDel_clicked();
    void on_rbVariableDel_clicked();
    void on_rbDeviceDel_clicked();
    void on_cbVariable_currentTextChanged(const QString &arg1);
    void on_cbDevice_currentTextChanged(const QString &arg1);
    void on_cbClass_currentTextChanged(const QString &arg1);
    void on_cbUsing_currentTextChanged(const QString &arg1);

private:
    void enable( int type);
    Ui::DeviceDlg *ui;
    FlowIt* m_fi;
};

#endif // DEVICEDLG_H
