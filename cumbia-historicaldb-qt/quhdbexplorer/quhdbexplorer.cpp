#include "quhdbexplorer.h"
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
#include <QCheckBox>
#include <QSettings>


#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cumbiatango.h>
#include <cutango-world.h>
#include <cutreader.h>
#include <cutcontrolsreader.h>
#include <cucontext.h>
#include <quhdbbrowser.h>
#endif

QuHdbExplorer::QuHdbExplorer(QWidget *parent)
    : QWidget(parent)
{
    m_cupool = new CumbiaPool();
    QString db_profile;
    foreach(QString a, qApp->arguments()) {
        if(a.startsWith("--db-profile=")) {
            db_profile = a.remove("--db-profile=");
        }
    }
    CuPluginLoader pl;
    QObject *o;
    CuHdbPlugin_I *hdb_p = pl.get<CuHdbPlugin_I>("cuhdb-qt-plugin.so", &o);
    if(hdb_p) {
        m_cupool->registerCumbiaImpl("hdb", hdb_p->getCumbia());
        m_cupool->setSrcPatterns("hdb", hdb_p->getSrcPatterns());
        m_fpool.registerImpl("hdb", *hdb_p->getReaderFactory());
    }
    if(!db_profile.isEmpty()) {
            hdb_p->setDbProfile(db_profile);
    }
    else {
        printf("\e[1;33m* \e[0musing \e[1;33mdefault\e[0m database profile, if available\n");
    }

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    CumbiaTango *cut = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    m_cupool->registerCumbiaImpl("tango", cut);
    m_cupool->setSrcPatterns("tango", CuTangoWorld().srcPatterns());
    m_fpool.registerImpl("tango", CuTReaderFactory());
    m_fpool.setSrcPatterns("tango", CuTangoWorld().srcPatterns());
#endif

    QSettings s;
    QDateTimeEdit *t1 = new QDateTimeEdit(this);
    QDateTimeEdit *t2 = new QDateTimeEdit(this);
    t1->setObjectName("dte1");
    t2->setObjectName("dte2");
    s.contains("STOP_DT") ? t2->setDateTime(s.value("STOP_DT").toDateTime()) :
                            t2->setDateTime(QDateTime::currentDateTime());
    s.contains("START_DT") ? t1->setDateTime(s.value("START_DT").toDateTime()) :
        t1->setDateTime(t2->dateTime().addDays(-1));
    t1->setDisplayFormat("yyyy.MM.dd hh.mm.ss");
    t1->setCalendarPopup(true);
    t2->setDisplayFormat("yyyy.MM.dd hh.mm.ss");
    t2->setCalendarPopup(true);

    QPushButton *pbGet = new QPushButton("Get", this);
    QPushButton *pbNow = new QPushButton("Now", this);
    QPushButton *pbClear = new QPushButton("Clear", this);
    QLineEdit *lesrc = new QLineEdit(this);
    lesrc->setObjectName("lesrc");
    lesrc->setPlaceholderText("Type one or more space separated source names, e.g. sys/tg_test/1/double_scalar");
    QuTrendPlot *plot = new QuTrendPlot(this, m_cupool, m_fpool);
    plot->setYAxisAutoscaleEnabled(true);
    plot->setShowDateOnTimeAxis(true);
    plot->setUpperBoundExtra(QwtPlot::xBottom, 0.05);
    plot->setUpperBoundExtra(QwtPlot::yLeft, 0.2);

    QLineEdit *leNotes = new QLineEdit(this);
    leNotes->setObjectName("notes");
    connect(plot, SIGNAL(markerTextChanged(QString)), leNotes, SLOT(setText(QString)));
    connect(plot, SIGNAL(markerVisibilityChanged(bool)), leNotes, SLOT(setVisible(bool)));
    leNotes->setVisible(false);

    QuHdbBrowser *hdbbro = new QuHdbBrowser(this, m_cupool, m_fpool);
    QCheckBox *cbScalar = new QCheckBox("Scalars", this);
    QCheckBox *cbSpectrum = new QCheckBox("Spectrum", this);
    cbScalar->setChecked(true);
    cbScalar->setObjectName("cbScalar");
    connect(cbScalar, SIGNAL(toggled(bool)), this, SLOT(reloadTree()));
    connect(cbSpectrum, SIGNAL(toggled(bool)), this, SLOT(reloadTree()));
    cbSpectrum->setObjectName("cbSpectrum");
    QLineEdit *leFilter = new QLineEdit(this);
    leFilter->setObjectName("leFilter");
    leFilter->setPlaceholderText("filter and click on Reload");
    QPushButton *pbReloadTree = new QPushButton("Reload", this);
    connect(pbReloadTree, SIGNAL(clicked()), this, SLOT(reloadTree()));

    int col = 0;
    QGridLayout *glo = new QGridLayout(this);
    glo->addWidget(cbScalar, 0, col, 1, 1);
    glo->addWidget(cbSpectrum, 0, col + 1, 1, 1);
    glo->addWidget(pbReloadTree, 0, col + 2, 2, 1);
    glo->addWidget(leFilter, 1, col, 1, 2);

    glo->addWidget(hdbbro, 2, col, 6, 3);
    col += 3;
    glo->addWidget(leNotes, 0, 0, 1, 7 + col);
    glo->addWidget(plot, 1, col + 0, 5, 7);
    int row = 6;
    glo->addWidget(new QLabel("Start and stop date", this), row, col + 0, 1 , 2);
    glo->addWidget(t1, row, col + 2, 1, 1);
    glo->addWidget(t2, row, col + 3, 1, 1);
    glo->addWidget(pbNow, row, col + 4, 1, 1);
    glo->addWidget(pbGet, row, col + 5, 1, 1);
    glo->addWidget(pbClear, row, col + 6, 1, 1);
    glo->addWidget(lesrc, 7, 0, 1, col + 6);

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    QCheckBox *cb = new QCheckBox("Read live", this);
    cb->setObjectName("cbLive");
    cb->setChecked(s.value("READ_LIVE", false).toBool());
    glo->addWidget(cb, 7, col + 6, 1, 1);
#endif

    lesrc->setText(s.value("SOURCES", "").toString());
    lesrc->setToolTip("Insert a space separated list of sources");

    connect(pbNow, SIGNAL(clicked()), this, SLOT(setT2Now()));
    connect(pbGet, SIGNAL(clicked()), this, SLOT(get()));
    connect(pbClear, SIGNAL(clicked()), plot, SLOT(clearPlot()));

    reloadTree();
}

QuHdbExplorer::~QuHdbExplorer()
{
    QSettings s;
    s.setValue("SOURCES", findChild<QLineEdit *>("lesrc")->text());
    s.setValue("START_DT", findChild<QDateTimeEdit *>("dte1")->dateTime());
    s.setValue("STOP_DT", findChild<QDateTimeEdit *>("dte2")->dateTime());
    s.setValue("READ_LIVE", findChild<QCheckBox *>("cbLive")->isChecked());
}

void QuHdbExplorer::setT2Now()
{
    findChild<QDateTimeEdit *>("dte2")->setDateTime(QDateTime::currentDateTime());
}

void QuHdbExplorer::get() {
    QDateTime t1 = findChild<QDateTimeEdit *>("dte1")->dateTime();
    QDateTime t2 = findChild<QDateTimeEdit *>("dte2")->dateTime();
    QStringList srcs = findChild<QLineEdit *>("lesrc")->text().split(QRegExp("\\s+"));
    QStringList live;

    for(int i = 0; i < srcs.size(); i++) {
        if(findChild<QCheckBox *>("cbLive") && findChild<QCheckBox *>("cbLive")->isChecked())
            live << srcs[i];
        srcs[i] = "hdb://" + srcs[i] + + "(" +
                t1.toString("yyyy-MM-dd hh:mm:ss") + "," + t2.toString("yyyy-MM-dd hh:mm:ss") + ")";
    }
    if(t2 > t1) {
        findChild<QuTrendPlot *>()->getContext()->setOptions(CuData("refresh_mode", CuTReader::PolledRefresh));
        findChild<QuTrendPlot *>()->setSources(srcs + live);
    }
}

void QuHdbExplorer::reloadTree()
{
    bool scalar = findChild<QCheckBox *>("cbScalar")->isChecked();
    bool spectrum = findChild<QCheckBox *>("cbSpectrum")->isChecked();
    QString filter = findChild<QLineEdit *>("leFilter")->text();
    QString q;
    if(spectrum ^ scalar) {
        q = "select att_name from att_conf,att_conf_data_type "
                        "WHERE att_conf.att_conf_data_type_id=att_conf_data_type.att_conf_data_type_id "
                        "AND att_conf_data_type.data_type like ";
        spectrum ? q += "'%array%'": q += "'%scalar%'";
    }
    else if(spectrum && scalar) {
        q = "select att_name from att_conf,att_conf_data_type WHERE "
                "att_conf.att_conf_data_type_id=att_conf_data_type.att_conf_data_type_id "
                "AND (att_conf_data_type.data_type like '%scalar%' OR att_conf_data_type.data_type "
                "like '%array%')";
    }
    if(!filter.isEmpty())
        q += " AND att_conf.att_name like '%%1%'" + filter;

    findChild<QuHdbBrowser *>()->setSource("hdb://query/" + q);
}

