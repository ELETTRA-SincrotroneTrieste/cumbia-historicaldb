#ifndef QUHDBBROWSER_H
#define QUHDBBROWSER_H

#include <QWidget>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>

QT_BEGIN_NAMESPACE
namespace Ui { class QuHdbBrowser; }
QT_END_NAMESPACE

class QuHdbBrowser : public QWidget
{
    Q_OBJECT

public:
    QuHdbBrowser(QWidget *parent);
    ~QuHdbBrowser();

private slots:
    void setT2Now();
    void get();

private:
    CumbiaPool *m_cupool;
    CuControlsFactoryPool m_fpool;

};
#endif // QUHDBBROWSER_H
