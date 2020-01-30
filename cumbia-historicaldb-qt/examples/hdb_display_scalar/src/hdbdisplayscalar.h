#ifndef HdbDisplayScalar_H
#define HdbDisplayScalar_H

#include <QWidget>
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
#include <cumbiatango.h>
#include <cumbiaepics.h>

namespace Ui {
class HdbDisplayScalar;
}

class CuData;
class CumbiaPool;

class HdbDisplayScalar : public QWidget
{
    Q_OBJECT

public:
    explicit HdbDisplayScalar(CumbiaPool *cu_p, QWidget *parent = 0);
    ~HdbDisplayScalar();

private slots:
    void getData();

private:
    Ui::HdbDisplayScalar *ui;

    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
};

#endif // HdbDisplayScalar_H
