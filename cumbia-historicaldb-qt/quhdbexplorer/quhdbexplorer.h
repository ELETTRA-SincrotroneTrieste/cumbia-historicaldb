#ifndef QUHDEXPLORER_H
#define QUHDEXPLORER_H

#include <QWidget>
#include <cumbiapool.h>
#include <cucontrolsfactorypool.h>

QT_BEGIN_NAMESPACE
namespace Ui { class QuHdbBrowser; }
QT_END_NAMESPACE

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

private:
    CumbiaPool *m_cupool;
    CuControlsFactoryPool m_fpool;

};
#endif // QUHDBBROWSER_H
