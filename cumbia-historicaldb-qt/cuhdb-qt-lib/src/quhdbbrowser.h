#ifndef QUHDBBROWSER_H
#define QUHDBBROWSER_H

#include <QTreeWidget>
#include <cudatalistener.h>
#include <QThread>

class CumbiaPool;
class CuControlsFactoryPool;
class QuHdbBrowserPrivate;

class TreeBuilder : public QThread {
    Q_OBJECT
public:
    TreeBuilder(const QStringList& srclist);

    void run();

    QStringList srcs;
};

class QuHdbBrowser : public QTreeWidget, public CuDataListener
{
    Q_OBJECT
public:
    QuHdbBrowser(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool);
    ~QuHdbBrowser();

    void onUpdate(const CuData &data);

    QString source() const;

    void updateSourcesList(const QStringList &sources, bool expand);
    void addSource(const QString &src, bool expand);
    QTreeWidgetItem *findChild(QTreeWidgetItem *parent, const QString &name);
    QStringList selectedSources() const;
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    QMimeData *mimeData(const QList<QTreeWidgetItem *> items) const;
    int level(QTreeWidgetItem *it) const;
    QString buildItemText(const QTreeWidgetItem *it) const;
    void removeSelectedSources();
    QStringList getSources() const;

public slots:
    void setSource(const QString& s);
private:
    QuHdbBrowserPrivate *d;
};

#endif // QUHDBBROWSER_H
