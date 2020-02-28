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
#include <QRadioButton>
#include <QtDataVisualization/Q3DSurface>


#ifdef QUMBIA_TANGO_CONTROLS_VERSION
#include <cumbiatango.h>
#include <cutango-world.h>
#include <cutreader.h>
#include <cutcontrolsreader.h>
#include <cucontext.h>
#include "quhdbbrowser.h" // unix:INCLUDEPATH in .pro
#ifdef HAS_QUTIMEARRAY3D

// the following is provided by the qutimearray3dplotplugin plugin
#include <qutimearray3dplotplugin_i.h>
#endif

#endif

QuHdbExplorer::QuHdbExplorer(QWidget *parent)
    : QWidget(parent)
{
    m_surface = nullptr;
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

    m_ta3d_plugin = nullptr;
#ifdef HAS_QUTIMEARRAY3D
    QObject *o3d;
    m_ta3d_plugin = pl.get<QuTimeArray3DPlotPlugin_I>("qutimearray3dplotplugin.so", &o3d);
#endif

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    CumbiaTango *cut = new CumbiaTango(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
    m_cupool->registerCumbiaImpl("tango", cut);
    m_cupool->setSrcPatterns("tango", CuTangoWorld().srcPatterns());
    m_fpool.registerImpl("tango", CuTReaderFactory());
    m_fpool.setSrcPatterns("tango", CuTangoWorld().srcPatterns());
#endif

    // grid layout
    QGridLayout *glo = new QGridLayout(this);
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
    pbClear->setObjectName("pbClearPlot");
    QLineEdit *lesrc = new QLineEdit(this);
    lesrc->setObjectName("lesrc");
    lesrc->setPlaceholderText("Type one or more space separated source names, e.g. sys/tg_test/1/double_scalar");
    QPushButton *pbClearSrc = new QPushButton("Clear", this);
    connect(pbClearSrc, SIGNAL(clicked()), lesrc, SLOT(clear()));
    QuTrendPlot *plot = m_createTrendPlot();

    QLineEdit *leNotes = new QLineEdit(this);
    leNotes->setObjectName("notes");
    connect(plot, SIGNAL(markerTextChanged(QString)), leNotes, SLOT(setText(QString)));
    connect(plot, SIGNAL(markerVisibilityChanged(bool)), leNotes, SLOT(setVisible(bool)));
    leNotes->setVisible(false);

    QuHdbBrowser *hdbbro = new QuHdbBrowser(this, m_cupool, m_fpool);
    QRadioButton *rbScalar = new QRadioButton("Scalars", this);
    rbScalar->setChecked(true);
    rbScalar->setObjectName("rbScalar");
    connect(rbScalar, SIGNAL(toggled(bool)), this, SLOT(reloadTree()));
    QLineEdit *leFilter = new QLineEdit(this);
    leFilter->setObjectName("leFilter");
    leFilter->setPlaceholderText("filter and click on Reload");
    leFilter->setText(s.value("TREE_FILTER_TEXT", "").toString());
    QPushButton *pbReloadTree = new QPushButton("Reload", this);
    connect(pbReloadTree, SIGNAL(clicked()), this, SLOT(reloadTree()));

    int col = 0;
    glo->addWidget(rbScalar, 0, col, 1, 1);

    // spectrum support, only if qutimearray3dplot plugin is available
    if(m_ta3d_plugin != nullptr) {
        QRadioButton *rbSpectrum = new QRadioButton("Spectrum", this);
        rbSpectrum->setObjectName("rbSpectrum");
        connect(rbSpectrum, SIGNAL(toggled(bool)), this, SLOT(reloadTree()));
        glo->addWidget(rbSpectrum, 0, col + 1, 1, 1);
        QPushButton *pbConfPlot = new QPushButton("Configure plot", this);
        pbConfPlot->setObjectName("pbConfPlot");
        pbConfPlot->setCheckable(true);
        connect(pbConfPlot, SIGNAL(toggled(bool)), this, SLOT(show3DPlotConf(bool)));
        glo->addWidget(pbConfPlot, 0, col + 2, 1, 1);
    }

    glo->addWidget(pbReloadTree, 1, col + 2, 1, 1);
    glo->addWidget(leFilter, 1, col, 1, 2);

    glo->addWidget(hdbbro, 2, col, 5, 3);
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
    glo->addWidget(lesrc, 7, 0, 1, col + 5);
    glo->addWidget(pbClearSrc, 7, col + 5, 1, 1);

#ifdef QUMBIA_TANGO_CONTROLS_VERSION
    QCheckBox *cb = new QCheckBox("Read live", this);
    cb->setObjectName("cbLive");
    cb->setChecked(s.value("READ_LIVE", false).toBool());
    glo->addWidget(cb, 7, col + 6, 1, 1);
#endif

    lesrc->setText(s.value("SOURCES", "").toString());
    lesrc->setToolTip("Insert a space separated list of sources");
    QRadioButton *rbSpectrum = findChild<QRadioButton *>("rbSpectrum");
    if(rbSpectrum && s.value("SPECTRUM_VIEW", false).toBool())
        rbSpectrum->setChecked(true);
    else
        rbScalar->setChecked(true);

    connect(pbNow, SIGNAL(clicked()), this, SLOT(setT2Now()));
    connect(pbGet, SIGNAL(clicked()), this, SLOT(get()));

    reloadTree();

    resize(1000, 800);
}

QuHdbExplorer::~QuHdbExplorer()
{
    QSettings s;
    s.setValue("SOURCES", findChild<QLineEdit *>("lesrc")->text());
    s.setValue("START_DT", findChild<QDateTimeEdit *>("dte1")->dateTime());
    s.setValue("STOP_DT", findChild<QDateTimeEdit *>("dte2")->dateTime());
    s.setValue("READ_LIVE", findChild<QCheckBox *>("cbLive")->isChecked());
    s.setValue("TREE_FILTER_TEXT", findChild<QLineEdit *>("leFilter")->text());
    s.setValue("SPECTRUM_VIEW", !findChild<QRadioButton *>("rbScalar")->isChecked());
}

void QuHdbExplorer::setT2Now()
{
    findChild<QDateTimeEdit *>("dte2")->setDateTime(QDateTime::currentDateTime());
}

void QuHdbExplorer::get() {
    QDateTime t1 = findChild<QDateTimeEdit *>("dte1")->dateTime();
    QDateTime t2 = findChild<QDateTimeEdit *>("dte2")->dateTime();
    QString t = findChild<QLineEdit *>("lesrc")->text();
    QStringList srcs = t.trimmed().split(QRegExp("\\s+"));
    QStringList live;

    for(int i = 0; i < srcs.size(); i++) {
        if(findChild<QCheckBox *>("cbLive") && findChild<QCheckBox *>("cbLive")->isChecked())
            live << srcs[i];
        srcs[i] = "hdb://" + srcs[i] + + "(" +
                t1.toString("yyyy-MM-dd hh:mm:ss") + "," + t2.toString("yyyy-MM-dd hh:mm:ss") + ")";
    }
    if(t2 > t1) {
        QuTrendPlot *tp = findChild<QuTrendPlot *>();
        if(tp) {
            tp->getContext()->setOptions(CuData("refresh_mode", CuTReader::PolledRefresh));
            tp->setSources(srcs + live);
        }
#ifdef HAS_QUTIMEARRAY3D
        else if(m_surface && srcs.size() > 0) {
            QMetaObject::invokeMethod(m_surface, "clear");
            foreach(QString s, srcs) {
                QuTimeArrayReader_I *tarr_r = m_ta3d_plugin->createReader(this, m_cupool, m_fpool);
                tarr_r->setLink(s, m_surface);
            }
        }
#endif
    }
}

void QuHdbExplorer::reloadTree()
{
    QRadioButton *rb = findChild<QRadioButton *>("rbScalar");
    bool scalar = rb && rb->isChecked();
    bool spectrum = !scalar;
    QPushButton *confB = findChild<QPushButton *>("pbConfPlot");
    if(confB) confB->setVisible(spectrum);
    QString filter = findChild<QLineEdit *>("leFilter")->text();
    QString q;
    if(spectrum ^ scalar) {
        q = "select att_name from att_conf,att_conf_data_type "
            "WHERE att_conf.att_conf_data_type_id=att_conf_data_type.att_conf_data_type_id "
            "AND att_conf_data_type.data_type like ";
        spectrum ? q += "'%array%'": q += "'%scalar%'";
    }
    QGridLayout *glo = findChild<QGridLayout *>();
    QWidget *plot = findChild<QuTrendPlot *>();
    if(!plot)
        plot = findChild<QWidget *>("3dplot_container");
    int plot_layout_idx = glo->indexOf(plot);
    int layout_row, layout_col, lo_rowspan, lo_colspan;
    glo->getItemPosition(plot_layout_idx, &layout_row, &layout_col, &lo_rowspan, &lo_colspan);
    if(spectrum && plot->objectName() != "3dplot_container") {
        if(plot) { // plot but not surface
            glo->removeWidget(plot);
            delete plot;
        }
#ifdef HAS_QUTIMEARRAY3D
        if(m_ta3d_plugin)  {
            m_surface = m_ta3d_plugin->create("QuTimeArray3DPlot", nullptr);
            QWidget *container = QWidget::createWindowContainer(m_surface, this);
            container->setObjectName("3dplot_container");
            glo->addWidget(container, layout_row, layout_col, lo_rowspan, lo_colspan);
            QPushButton *pbClearPlot = findChild<QPushButton *>("pbClearPlot");
            connect(pbClearPlot, SIGNAL(clicked()), m_surface, SLOT(clear()));
        }
#endif
    }
    else if(scalar && qobject_cast<QuTrendPlot *>(plot) == nullptr) {
        if(plot) { // plot but not QuTrendPlot
            glo->removeWidget(plot);
            if(m_surface) {
                delete m_surface;
                m_surface = nullptr;
            }
            delete plot;
        }
        QuTrendPlot* trendp = m_createTrendPlot();
        glo->addWidget(trendp, layout_row, layout_col, lo_rowspan, lo_colspan);
    }
    else {
        qDebug() << "scalar" << scalar << "spectrum" << spectrum << "but plot already " << plot;
    }
    if(!filter.isEmpty())
        q += QString(" AND att_conf.att_name like '%%1%'").arg(filter);

    findChild<QuHdbBrowser *>()->setSource("hdb://query/" + q);
}

void QuHdbExplorer::show3DPlotConf(bool show) {
#ifdef HAS_QUTIMEARRAY3D
    if(m_ta3d_plugin) {
        QWidget *confw;
        show ? confw = m_ta3d_plugin->plotConfigurationWidget(m_surface) : confw = nullptr;
        if(confw)
            confw->setObjectName("3dplot_confw");
        QGridLayout *glo = findChild<QGridLayout *>();
        QWidget *plot = findChild<QWidget *>("3dplot_container");
        if(plot) {
            int r,c, rs, cs, plotidx = glo->indexOf(plot);
            glo->getItemPosition(plotidx, &r, &c, &rs, &cs);
            if(show) {
                glo->removeWidget(plot);
                glo->addWidget(plot, r, c, rs, cs-1);
                glo->addWidget(confw, r, c + cs -1, rs, 1);
            }
            else {
                if(findChild<QWidget *>("3dplot_confw")) {
                    delete findChild<QWidget *>("3dplot_confw");
                    glo->removeWidget(plot);
                    glo->addWidget(plot, r, c, rs, cs + 1);

                }
            }
        }
    }
#endif
}

QuTrendPlot *QuHdbExplorer::m_createTrendPlot()
{
    QuTrendPlot *plot = findChild<QuTrendPlot *>();
    if(plot)
        delete plot;
    plot = new QuTrendPlot(this, m_cupool, m_fpool);
    plot->setYAxisAutoscaleEnabled(true);
    plot->setShowDateOnTimeAxis(true);
    QPushButton *pbClearPlot = findChild<QPushButton *>("pbClearPlot");
    connect(pbClearPlot, SIGNAL(clicked()), plot, SLOT(clearPlot()));
    return plot;
}

