#ifndef QUHDEXPLORER_H
#define QUHDEXPLORER_H

#include <QWidget>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>

QT_BEGIN_NAMESPACE
namespace Ui { class QuHdbBrowser; }
QT_END_NAMESPACE

class QuTimeArray3DPlotPlugin_I;
class QuTrendPlot;
class QWindow;

class QuHdbExplorer : public QWidget
{
    Q_OBJECT

public:
    QuHdbExplorer(QWidget *parent);
    ~QuHdbExplorer();

private slots:
    void setT2Now();
    void get();
    void reloadTree();
    void show3DPlotConf(bool show);

private:
    CumbiaPool *m_cupool;
    CuControlsFactoryPool m_fpool;
    QuTimeArray3DPlotPlugin_I *m_ta3d_plugin;
    QWindow *m_surface;
    QuTrendPlot *m_createTrendPlot();

};
#endif // QUHDBBROWSER_H
