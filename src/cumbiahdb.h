#ifndef CUMBIAHDB_H
#define CUMBIAHDB_H

#include <cumbia.h>
#include <string>
#include <cuhdbactionreader.h>
#include <cuhdbactionfactoryi.h>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CuDataListener;
class CumbiaHdbPrivate;
class HdbXSettings;

/*!
 * \mainpage Cumbia hdb module
 *
 * \section introduction Introduction
 *
 * The CumbiaHdb module offers access to the *hdb++* historical database used by the Tango
 * control system.
 *
 * \subsection dependencies Prerequisites
 *
 * Dependencies are located through the *PKG_CONFIG_VARIABLE* that must be exported
 * appropriately before compilation.
 *
 * To build the base library, you need the *hdbextractor* and *cumbia* dependencies:
 *
 * - [hdbextractor library](https://github.com/ELETTRA-SincrotroneTrieste/hdbextractor)
 * - [cumbia-libs](https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs)
 *
 * If the installation prefix of both libraries is /usr/local/cumbia-libs, then remember to
 * \code export PKG_CONFIG_PATH=/usr/local/cumbia-libs/lib/pkgconfig:/usr/local/hdbextractor/lib/pkgconfig
 * \endcode
 *
 * \subsubsection qt_integration Qt integration
 *
 * Change directory into *cumbia-historicaldb-qt* and execute
 * - qmake  INSTALL_ROOT=/path/to/cumbia-libs
 * - make
 * - make install
 * to install the Qt modules.
 *
 * The following components will be built and installed:
 * - *cuhdb-qt-lib* a small library offering Qt ready to use and *cumbia* aware *readers*.
 *   A convenient *database browser* is provided as a ready to use *tree widget*.
 *
 * - *cuhdb-qt-plugin* a Qt plugin to integrate *cumbia-hdb* into an application as a *plugin*
 *
 * - *hdb-db-profile-manager* a small app to manage database configuration through [profiles](db_profiles)
 *
 * - *quhdbexplorer* a graphical application to display historical data in plots over time.
 *
 *   Consider installing the [qutimearray3dplotplugin](https://github.com/ELETTRA-SincrotroneTrieste/qutimearray3dplotplugin)
 *   to enable spectrum data representation over time (in a 3D surface)
 *
 * \subsubsection optional_plugins Optional components
 * Consider installing the [qutimearray3dplotplugin](https://github.com/ELETTRA-SincrotroneTrieste/qutimearray3dplotplugin)
 * to be able to represent spectrum data over time (in a 3D surface)
 *
 * \par Note
 * To build the qt components, you need the cumbia-qtcontrols module from the *cumbia-libs*,
 * that is installed by default by the *cumbia-libs* installer *scripts/cubuild.sh*.
 *
 *
 *
 *
 * \subsection supported_dbs Supported databases
 *
 * The hdb and hdb++ databases are supported through the *hdbextractor* library, although
 * the older hdb schema only partially.
 *
 * \subsection data_queries Data queries
 *
 * \subsection custom_queries Custom queries
 *
 * \subsection list_atts List of attributes stored into the database
 *
 * \subsection db_profiles Database profiles
 *
 * To let applications work without a specific database configuration file,
 * a *database profile* solution is adopted by cumbia-hdb apps.
 * *cumbia-hdb* manages several database *profile* configurations, that can be configured
 * by an utility installed alongside the *qt* modules: the *hdb-db-profile-manager*
 * (source under: cumbia-historicaldb-qt/hdb-db-profile-manager).
 *
 * Each *cumbia-hdb* application can load a *profile* rather than a
 * file, making it easier to administer system wide historical configurations.
 * For example:
 *
 * \code
    QString dbprofile;
    if(argc > 1) dbprofile = QString(argv[1]); // profile specified as option
    if(!db_profile.isEmpty()) {
        hdb_p->setDbProfile(db_profile);
    }
    else {
        // use default profile, see `hdb-db-profile-manager -l`
    }
 * \endcode
 *
 * \subsubsection hdb-db-profile-manager app
 *
 * Execute the *hdb-db-profile-manager* utility with the following parameters:
 * - *-a, --add-profile <profile>* to add a new profile
 * - *-e, --edit-profile <profile>* to edit an existing profile
 * - *-r, --remove-profile <profile>* to remove an existing profile
 * - *-d, --set-default* to set the default profile
 * - *-l, --list-profiles* to list the available profiles
 * - *<profile>* show the configuration of the given profile
 *
 * The *-a and -e* options offer a guided procedure to edit the configuration
 * of the database for *hdbextractor* and *cumbia-hdb* applications.
 *
 *
 *
 * \subsection cudata CuData contents from the cumbia-hdb module
 *
 * |Key          | Type       | Value             | Description   | Notes      |
 * |-------------|------------|-------------------|---------------|------------|
 * | src         | string     | the name of the source | The complete name of the source, without <em>hdb://</em> domain prefix and start/stop date | eg: *tango://tango-host:PORT/test/device/tg_test/double_scalar*        |
 * | time_scale_us | vector<double> | vector of double: seconds.microseconds |  `data_time` columns from the hdb++ database, MySQL type DATATIME | convert with toDouble. Note for Qt: use QDateTime::fromMSecsSinceEpoch(v[i] * 1000).toString("yyyy-MM-dd hh:mm:ss.zzz")) |
 * |notes_time_scale_us | std::vector<double> | vector of double: seconds.microseconds | `data_time` columns from the hdb++ database, MySQL type DATATIME, WHERE value is NULL | Conversion notes as time_scale_us |
 * |notes             | std::vector<std::string>  | vector of strings | error messages from the database. i-th notes_time_scale_us --> i-th note | convert with toStringVector |
 * |timestamp_ms      | long int        | timestamp, in milliseconds | timestamp of the single value fetched from the database | If data size from the db is > 1, this field *is not present* |
 * |timestamp_us      | double          | timestamp, double, seconds.microseconds | same semantics as timestamp_ms | Present if data size == 1 |
 * |data_format_str   | string          | "scalar" or "vector"   | The format of data | convert with toString |
 * |write_mode_str    | string          | "ro" or "rw"           | The write mode, either read only (ro) or read write (rw) | convert with toString |
 * |data_type_str     | string          | one of "double", "int", "uint", "bool", "string" | Type of data stored in *value* | use the type to choose the appropriate conversion method for *value* and *w_value* |
 * |value             | vector of *one of* double,int,uint,bool,string | the data value | Tango "read" value from the historical database | convert with the appropriate toXXXVector according to data_type_str |
 * |w_value           | same as value | the *Tango* *write* value (set point), if write_mode_str is "rw" | use data_type_str to convert to a vector of the appropriate type | Available for read/write attributes |
 * |err               | bool          | true or false | true if an error occurred during the database extraction process | the error condition concerns only the data extraction, not possible *null* values in the database. For the quality of the data recorded into the database, refer to *notes_time_scale_us, notes, quality_color, quality* |
 * |msg               | string        | a string message | any kind of message originated from the extraction, typically an error description | use in conjunction with *err* |
 * |has_null          | bool          | true if at least one value from the database is null (error condition during the save process) | see also *null_cnt*, *valid_cnt*, *notes_time_scale_us, notes* | |
 * |null_cnt          | int           | number of null values in the result | notes_time_scale_us.size, as int | convert with toInt |
 * |valid_cnt         | int           | number of valid values in the result| time_scale_us.size | convert with toInt |
 * |start_date        | string        | lower boundary of time interval | start date time | convert with toString. Format: yyyy-MM-dd hh:mm:ss |
 * |stop_date         | string        | upper boundary of time interval | stop date time | convert with toString. Format: yyyy-MM-dd hh:mm:ss  |
 * |elapsed           | double        | elapsed time, seconds.microseconds| time spent by the database query | *optional field * |
 * |columns           | std::vector<std::string> | name of the columns involved in a *custom query* | list of columns in database result | present in *custom queries* only. Use toStringVector |
 * |column_count      | size_t aka long int | number of columns in a *custom query* result | number of columns in the result | present in *custom queries* only. Use toLongInt |
 * |results           | std::vector<std::string> | the results of the query, row after row | in *custom queries*, stores the results in a sequence of rows one after another. Use column_count to separate each row |  each row of results is inserted at the back. Use toStringVector. Use in conjunction with *row_count* |
 * |row_count         | size_t aka long int | size of the vector of results | number of results  | use toLongInt |
 *
 * \subsection example Example
 *
 * The example discussed below can be found under the *cumbia-historicaldb-qt/examples/hdb_display_scalar* directory of the software distribution.
 *
 * Create a new project with the command
 * \code
   $cumbia new project
 * \endcode
 *
 * - select *Multi engine* in the *Support* box;
 * - type *hdb_display_scalar* in the *Project Name* input text field;
 * - Choose or type the directory where the project skeleton will be created, in the example case
 * - Click on Create
 *
 * \note Given the simplicity of this example, we will not use the Qt designer to draw the
 * graphical user interface. The lines in the *hdb-display-scalar.pro* file concerning the UI
 * form can be removed.
 *
 * Include necessary dependencies in the file *hdb-display-scalar.pro*:
 *
 * \code
 * PKGCONFIG += cumbia-hdb
 * \endcode
 *
 * Now open the *hdbdisplayscalar.cpp* file and add the lines necessary to load the historical
 * database plugin:
 *
 * \code
    #include <cupluginloader.h>
    #include <cuhistoricaldbplugin_i.h>

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

 * \endcode
 *
 * Create a QuTrendPlot, a QuLabel, a QLineEdit to write the source name and three QDateTimeEdit
 * to use as a calendar to set the start/stop dates. The plot will show the values between a start
 * and a stop date, while the QuLabel will display the value that the quantity had at a given date
 * and time. A QPushButton will trigger data fetch on the database.
 * We will add a QCheckBox that, if checked, will let the application read live data from the source
 * and append it to the plot.
 * Place the objects into a QGridLayout:
 *
 * \code

    #include <QGridLayout>
    #include <qulabel.h>
    #include <qutrendplot.h>
    #include <QLabel>
    #include <QPushButton>
    #include <QDateTimeEdit>
    #include <QLineEdit>

    // .. at the end of HdbDisplayScalar::HdbDisplayScalar

    // setup graphical objects
    QGridLayout *glo = new QGridLayout(this);
    QLineEdit *les = new QLineEdit(this);
    les->setObjectName("lesrc");
    les->setPlaceholderText("Tango attribute aa/bb/cc/dd");
    QuTrendPlot *plot = new QuTrendPlot(this, cu_pool, m_ctrl_factory_pool);
    QDateTimeEdit *dte1 = new QDateTimeEdit(this);
    dte1->setObjectName("dte1");
    QDateTimeEdit *dte2 = new  QDateTimeEdit(this);
    dte2->setObjectName("dte2");
    dte2->setDateTime(QDateTime::currentDateTime());
    dte1->setDateTime(dte2->dateTime().addSecs(3600 * 8));
    QPushButton *pb = new QPushButton("Get Data", this);
    connect(pb, SIGNAL(clicked()), this, SLOT(getData()));
    QCheckBox *cbLive = new QCheckBox("Read Live", this);
    QuLabel *data_l = new QuLabel(this, cu_pool, m_ctrl_factory_pool);
    data_l->setObjectName("data_l");
    QDateTimeEdit *dte3 = new QDateTimeEdit(this);
    dte3->setObjectName("dte3");
    dte3->setDateTime(dte2->dateTime());
    foreach(QDateTimeEdit *dte, findChildren<QDateTimeEdit *>()) {
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
 * \endcode
 *
 * Now implement the *getData* slot:
 *
 \code
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
 \endcode
 *
*/
class CumbiaHdb : public Cumbia
{

public:
    enum Type { CumbiaHdbType = Cumbia::CumbiaUserType + 14 };
    enum DbType { Hdb, Hdbpp };

    CumbiaHdb(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaHdb();

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    void setDbProfile(const std::string& db_profile_name);
    void setHdbXSettings(HdbXSettings *hdb_settings);
    HdbXSettings *hdbXSettings() const;

    CuData getDbParams() const;

    virtual int getType() const;

    void addAction(const std::string &source, CuDataListener *l, const CuHdbActionFactoryI &f);
    void unlinkListener(const string &source, CuHdbActionI::Type t, CuDataListener *l);
    CuHdbActionI *findAction(const std::string &source, CuHdbActionI::Type t) const;
private:

    void m_init();
    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
    CumbiaHdbPrivate *d;
};


#endif // CUMBIAHDB_H
