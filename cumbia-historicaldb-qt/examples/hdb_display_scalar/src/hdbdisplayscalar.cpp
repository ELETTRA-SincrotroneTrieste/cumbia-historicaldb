#include "hdbdisplayscalar.h"
#include "ui_hdbdisplayscalar.h"

#include <cumbiapool.h>
#include <cumbiaepics.h>
#include <cumbiatango.h>
#include <cuepcontrolsreader.h>
#include <cuepcontrolswriter.h>
#include <cutcontrolsreader.h>
#include <cutcontrolswriter.h>
#include <cutango-world.h>
#include <cuepics-world.h>
#include <cuthreadfactoryimpl.h>
#include <cuserviceprovider.h>
#include <qthreadseventbridgefactory.h>
#include <cumacros.h>

#include <cupluginloader.h>
#include <cuhistoricaldbplugin_i.h>

#include <QGridLayout>
#include <qulabel.h>
#include <qutrendplot.h>
#include <QLabel>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QCheckBox>

HdbDisplayScalar::HdbDisplayScalar(CumbiaPool *cumbia_pool, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HdbDisplayScalar)
{
    cu_pool = cumbia_pool;
    // setup Cumbia pool and register cumbia implementations for tango and epics
    CumbiaEpics* cuep = new CumbiaEpics(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    CumbiaTango* cuta = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    cu_pool->registerCumbiaImpl("tango", cuta);
    cu_pool->registerCumbiaImpl("epics", cuep);
    m_ctrl_factory_pool.registerImpl("tango", CuTReaderFactory());
    m_ctrl_factory_pool.registerImpl("tango", CuTWriterFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpReaderFactory());
    m_ctrl_factory_pool.registerImpl("epics", CuEpWriterFactory());


    CuTangoWorld tw;
    m_ctrl_factory_pool.setSrcPatterns("tango", tw.srcPatterns());
    cu_pool->setSrcPatterns("tango", tw.srcPatterns());
    CuEpicsWorld ew;
    m_ctrl_factory_pool.setSrcPatterns("epics", ew.srcPatterns());
    cu_pool->setSrcPatterns("epics", ew.srcPatterns());

    QString db_profile;
    CuPluginLoader pl;
    QObject *o;
    CuHdbPlugin_I *hdb_p = pl.get<CuHdbPlugin_I>("cuhdb-qt-plugin.so", &o);
    if(hdb_p) {
        cu_pool->registerCumbiaImpl("hdb", hdb_p->getCumbia());
        cu_pool->setSrcPatterns("hdb", hdb_p->getSrcPatterns());
        m_ctrl_factory_pool.registerImpl("hdb", *hdb_p->getReaderFactory());
    }
    // use cumbia hdb profiles managed by the hdb-db-profile-manager utility
    if(!db_profile.isEmpty())
        hdb_p->setDbProfile(db_profile);
    else
        printf("\e[1;33m* \e[0musing \e[1;33mdefault\e[0m database profile, if available\n");

    // log
    cuta->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));
    cuep->getServiceProvider()->registerService(CuServices::Log, new CuLog(&m_log_impl));

    // setup graphical objects
    QGridLayout *glo = new QGridLayout(this);
    QLineEdit *les = new QLineEdit(this);
    les->setObjectName("lesrc");
    les->setPlaceholderText("Tango attribute aa/bb/cc/dd");
    QuTrendPlot *plot = new QuTrendPlot(this, cu_pool, m_ctrl_factory_pool);
    plot->setYAxisAutoscaleEnabled(true);
    QDateTimeEdit *dte1 = new QDateTimeEdit(this);
    dte1->setObjectName("dte1");
    QDateTimeEdit *dte2 = new  QDateTimeEdit(this);
    dte2->setObjectName("dte2");
    dte2->setDateTime(QDateTime::currentDateTime());
    dte1->setDateTime(dte2->dateTime().addSecs(-3600 * 8));
    QPushButton *pb = new QPushButton("Get Data", this);
    connect(pb, SIGNAL(clicked()), this, SLOT(getData()));
    QCheckBox *cbLive = new QCheckBox("Read Live", this);
    QuLabel *data_l = new QuLabel(this, cu_pool, m_ctrl_factory_pool);
    data_l->setObjectName("data_l");
    QDateTimeEdit *dte3 = new QDateTimeEdit(this);
    dte3->setObjectName("dte3");
    dte3->setDateTime(dte2->dateTime());
    foreach(QDateTimeEdit *dte, findChildren<QDateTimeEdit *>()) {
        dte->setDisplayFormat("yyyy.MM.dd hh.mm.ss");
        plot->setShowDateOnTimeAxis(true);
        dte->setCalendarPopup(true);
    }

    // layout
    glo->addWidget(les, 0, 0, 1, 7);
    glo->addWidget(pb, 0, 7, 1, 1);
    glo->addWidget(plot, 1, 0, 4, 7);
    glo->addWidget(dte1, 1, 7, 1, 1);
    glo->addWidget(dte2, 2, 7, 1, 1);
    glo->addWidget(cbLive, 3, 7, 1, 1);
    glo->addWidget(data_l, 7, 0, 1, 7);
    glo->addWidget(dte3, 7, 7, 1, 1);

    resize(800, 400);
}

HdbDisplayScalar::~HdbDisplayScalar()
{
    delete ui;
}

void HdbDisplayScalar::getData()
{
    findChild<QuTrendPlot *>()->clearPlot();
    QDateTimeEdit *dt1 = findChild<QDateTimeEdit *>("dte1");
    QDateTimeEdit *dt2 = findChild<QDateTimeEdit *>("dte2");
    QDateTimeEdit *dt3 = findChild<QDateTimeEdit *>("dte3");
    QString hdbsrc, src = findChild<QLineEdit *>()->text();
    hdbsrc = QString("hdb://%1(%2,%3)").arg(src).arg(dt1->dateTime().toString("yyyy-MM-dd hh:mm:ss")).
            arg(dt2->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    QStringList srcs = QStringList() << hdbsrc;
    if(findChild<QCheckBox *>()->isChecked())
        srcs << src;
    findChild<QuTrendPlot *>()->setSources(srcs);
    findChild<QuLabel *>()->setSource(QString("hdb://%1(%2)").arg(src).arg(dt3->dateTime().toString("yyyy-MM-dd hh:mm:ss")));
}
