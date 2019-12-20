#include "quhdbbrowser.h"
#include <qutrendplot.h>
#include <QDateTimeEdit>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QLineEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include <cupluginloader.h>
#include <cuhistoricaldbplugin_i.h>

QuHdbBrowser::QuHdbBrowser(QWidget *parent)
    : QWidget(parent)
{
    m_cupool = new CumbiaPool();
    QString db_profile;
    foreach(QString a, qApp->arguments()) {
        if(a.startsWith("--db-profile=")) {
            db_profile = a.remove("--db-profile=");
        }
    }
    if(!db_profile.isEmpty()) {
        CuPluginLoader pl;
        QObject *o;
        CuHdbPlugin_I *hdb_p = pl.get<CuHdbPlugin_I>("cuhdb-qt-plugin.so", &o);
        if(hdb_p) {
            hdb_p->setDbProfile(db_profile);
            m_cupool->registerCumbiaImpl("hdb", hdb_p->getCumbia());
            m_cupool->setSrcPatterns("hdb", hdb_p->getSrcPatterns());
            m_fpool.registerImpl("hdb", *hdb_p->getReaderFactory());
        }
    }
    else {
        QString msg("QuHdbBrowser: parameter --settings=dbconfig.dat is missing");
        QMessageBox::information(this, "Usage", msg);
        setDisabled(true);
    }
    QDateTimeEdit *t1 = new QDateTimeEdit(this);
    QDateTimeEdit *t2 = new QDateTimeEdit(this);
    t1->setObjectName("dte1");
    t2->setObjectName("dte2");
    t2->setDateTime(QDateTime::currentDateTime());
    t1->setDateTime(t2->dateTime().addDays(-1));
    t1->setDisplayFormat("yyyy.MM.dd hh.mm.ss");
    t2->setDisplayFormat("yyyy.MM.dd hh.mm.ss");
    QPushButton *pbGet = new QPushButton("Get", this);
    QPushButton *pbNow = new QPushButton("Now", this);
    QLineEdit *lesrc = new QLineEdit(this);
    QuTrendPlot *plot = new QuTrendPlot(this, m_cupool, m_fpool);
    plot->setYAxisAutoscaleEnabled(true);
    plot->setShowDateOnTimeAxis(true);
    plot->setUpperBoundExtra(QwtPlot::xBottom, 0);

    QGridLayout *glo = new QGridLayout(this);
    glo->addWidget(plot, 0, 0, 4, 6);
    glo->addWidget(new QLabel("Start and stop date", this), 4, 0, 1 , 2);
    glo->addWidget(t1, 4, 2, 1, 1);
    glo->addWidget(t2, 4, 3, 1, 1);
    glo->addWidget(pbNow, 4, 4, 1, 1);
    glo->addWidget(pbGet, 4, 5, 1, 1);
    glo->addWidget(lesrc, 5, 0, 1, 6);

    lesrc->setText("inj/power_supply/psch_inj.01/Current inj/power_supply/psch_inj.02/Current"
                   " inj/power_supply/pscv_inj.01/Current inj/power_supply/pscv_inj.02/Current");
    lesrc->setToolTip("Insert a space separated list of sources");

    connect(pbNow, SIGNAL(clicked()), this, SLOT(setT2Now()));
    connect(pbGet, SIGNAL(clicked()), this, SLOT(get()));
}

QuHdbBrowser::~QuHdbBrowser()
{
//    delete ui;
}

void QuHdbBrowser::setT2Now()
{
    findChild<QDateTimeEdit *>("dte2")->setDateTime(QDateTime::currentDateTime());
}

void QuHdbBrowser::get() {
    QDateTime t1 = findChild<QDateTimeEdit *>("dte1")->dateTime();
    QDateTime t2 = findChild<QDateTimeEdit *>("dte2")->dateTime();
    QStringList srcs = findChild<QLineEdit *>()->text().split(QRegExp("\\s+"));
    for(int i = 0; i < srcs.size(); i++) {
        srcs[i] = "hdb://" + srcs[i] + + "(" +
                t1.toString("yyyy-MM-dd hh:mm:ss") + "," + t2.toString("yyyy-MM-dd hh:mm:ss") + ")";
    }
    if(t2 > t1) {
        findChild<QuTrendPlot *>()->setSources(srcs);
    }
}

