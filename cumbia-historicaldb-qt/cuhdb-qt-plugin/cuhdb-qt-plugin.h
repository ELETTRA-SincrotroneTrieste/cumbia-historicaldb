#ifndef CUHDB_QT_PLUGIN_H
#define CUHDB_QT_PLUGIN_H

#include <QObject>
#include <cuhistoricaldbplugin_i.h>

class CuHdbQtPluginPrivate;
class Cumbia;
class CumbiaPool;
class CuControlsReaderFactoryI;

/** \mainpage
 *
 */
class CuHdbQtPlugin : public QObject, public CuHdbPlugin_I
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "cuhdb-qt-plugin.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(CuHdbPlugin_I)

public:
    CuHdbQtPlugin(QObject *parent = 0);

    virtual ~CuHdbQtPlugin();

    virtual Cumbia* getCumbia() const;

    virtual std::vector<std::string> getSrcPatterns() const;

    CuControlsReaderFactoryI* getReaderFactory() const;

    void setDbProfile(const QString &profile_name);

private:
    CuHdbQtPluginPrivate *d;
};

#endif // CUHDB_QT_PLUGIN_H
