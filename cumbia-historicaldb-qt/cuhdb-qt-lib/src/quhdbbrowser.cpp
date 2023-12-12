#include "quhdbbrowser.h"

#include <cumacros.h>
#include <cudata.h>
#include <QtDebug>
#include <cucontext.h>
#include <cucontrolsreader_abs.h>
#include <cumbiapool.h>
#include <cudata.h>
#include <qustring.h>
#include <qustringlist.h>
#include <cucontrolsfactories_i.h>
#include <cucontrolsfactorypool.h>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QtDebug>
#include <dbmacros.h>
#include <cumbiahdbworld.h>
#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QMimeData>

class QuHdbBrowserPrivate {
public:
    CuContext *ctx;
};

QuHdbBrowser::QuHdbBrowser(QWidget *parent, CumbiaPool *cumbia_pool, const CuControlsFactoryPool &fpool)  : QTreeWidget(parent)
{
    d = new QuHdbBrowserPrivate;
    d->ctx = new CuContext(cumbia_pool, fpool);

    setDragEnabled(true);
    setMouseTracking(true);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    setDragDropMode(QTreeView::DragDrop);
}

QuHdbBrowser::~QuHdbBrowser()
{
    delete d->ctx;
    delete d;
}

void QuHdbBrowser::onUpdate(const CuData &data)
{
    if(!data["err"].toBool() && data.containsKey("results")) {
        clear();
        CumbiaHdbWorld w;
        int rows = w.rowCount(data);
        std::vector<std::vector<std::string> > slices;
        w.slice(data, slices);
        std::vector<std::string> srcs;
        // srcs = data["results"].toStringVector();
        for(int i = 0; i < rows; i++) {
            srcs.push_back(w.getValue(data, slices, i, "att_name"));
        }
        printf("QuHdbBrowser.onUpdate: got %ld sources\n", srcs.size());
        updateSourcesList(QuStringList(srcs), false);
    }
    d->ctx->disposeReader();
}

QString QuHdbBrowser::source() const
{
    if(CuControlsReaderA* r = d->ctx->getReader())
        return r->source();
    return "";
}

void QuHdbBrowser::setSource(const QString &s)
{
    CuControlsReaderA * r = d->ctx->replace_reader(s.toStdString(), this);
    if(r)
        r->setSource(s);
}

QStringList QuHdbBrowser::getSources() const
{
    QStringList ret;
    QList<QTreeWidgetItem *> domItems = findItems("*", Qt::MatchWildcard);
    foreach(QTreeWidgetItem *domIt, domItems)
    {
        for(int i = 0; i < domIt->childCount(); i++)
        {
            QTreeWidgetItem *familyIt = domIt->child(i);
            for(int h = 0; h < familyIt->childCount(); h++)
            {
                QTreeWidgetItem *memberIt = familyIt->child(h);
                for(int l = 0; l < memberIt->childCount(); l++)
                {
                    QTreeWidgetItem *attIt = memberIt->child(l);
                    ret << attIt->data(0, Qt::UserRole).toString();
                }
            }
        }
    }
    qDebug() << "THdbConfigDialog::m_sourcesFromTree() " << ret;
    return ret;
}

void QuHdbBrowser::removeSelectedSources()
{
    foreach(QTreeWidgetItem *it, selectedItems())
        delete it;
}

QString QuHdbBrowser::buildItemText(const QTreeWidgetItem *it) const
{
  QString text;
  while(it)
  {
        text = ("/" + it->text(0)) + text;
        it = it->parent();
  }
  text.remove(0, 1); /* remove first / */
  return text;
}

int QuHdbBrowser::level(QTreeWidgetItem *it) const
{
  int lev = 0;
  QTreeWidgetItem *item = it;
  while(item)
  {
        lev++;
        item = item->parent();
  }
  return lev;
}

QMimeData * QuHdbBrowser::mimeData( const QList<QTreeWidgetItem *> items ) const
{
  if(items.size() != 1)
        return NULL;
  QMimeData *mimeData = new QMimeData();
  if(level(items.first()) >= 4)
  {
        QString text = buildItemText(items.first());
        mimeData->setText(text);
  }
  else
        return NULL;
  return mimeData;
}

void QuHdbBrowser::dragMoveEvent ( QDragMoveEvent * event )
{
   if (event->mimeData()->hasFormat("text/plain") ||
           event->mimeData()->hasFormat("text/xml"))
   {
         event->acceptProposedAction();
   }
}

void QuHdbBrowser::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << __FUNCTION__ << event->mimeData()->formats();
  if (event->mimeData()->hasFormat("text/plain") ||
          event->mimeData()->hasFormat("text/xml"))
        event->acceptProposedAction();
}

void QuHdbBrowser::dropEvent(QDropEvent *event) {
    qDebug() << __FUNCTION__ << event->mimeData()->formats();
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasText())  {
        QRegularExpression rx("([a-zA-z0-9_\\-\\.]+/[a-zA-z0-9_\\-\\.]+/[a-zA-z0-9_\\-\\.]+/[a-zA-z0-9_\\-\\.]+)");
        QString txt = mimeData->text();
        QStringList list;
        qDebug() << __FUNCTION__ << txt << "cntains rx " << txt.contains(rx);
        if(txt.contains(rx)) {
            int pos = 0;
            QRegularExpressionMatch ma = rx.match(txt, pos);
            while (ma.hasMatch()) {
                list << ma.captured(1);
                pos = ma.capturedEnd(1);
            }
        }
        foreach(QString src, list)
            addSource(src, true);
    }
    /* test/device/1/double_scalar */
}

QStringList QuHdbBrowser::selectedSources() const
{
    QStringList srcs;
    foreach(QTreeWidgetItem *it, selectedItems()) {
        QString src = it->data(0, Qt::UserRole).toString();
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        if(src.split('/', Qt::SkipEmptyParts).size() == level(it))
#else
        if(src.split('/', QString::SkipEmptyParts).size() == level(it))
#endif
            srcs << src;
    }

    return srcs;
}

QTreeWidgetItem* QuHdbBrowser::findChild(QTreeWidgetItem *parent, const QString& name)
{
    QTreeWidgetItem *ret = NULL;
    if(!parent)
    {
        QList<QTreeWidgetItem *> itList = findItems(name, Qt::MatchFixedString);
        if(itList.size())
            ret = itList.first();
    }
    else
    {
        for(int i = 0; i < parent->childCount(); i++)
        {
            if(!parent->child(i)->text(0).compare(name, Qt::CaseInsensitive))
            {
                ret = parent->child(i);
                break;
            }
        }
    }
    return ret;
}

void  QuHdbBrowser::addSource(const QString &src, bool expand)
{
    QString name = src;
    name.remove("tango://");
    QTreeWidgetItem* parent = NULL, *found = NULL;
    QString dom, fam, mem, att, facility, proto;
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    QStringList sl = name.split('/', Qt::SkipEmptyParts);
#else
    QStringList sl = name.split('/', QString::SkipEmptyParts);
#endif
    for(int i = 0; i < sl.size(); i++)
    {
        QString pt = sl.at(i);
        if(pt.endsWith("::"))
            pt.remove("::");
        found = findChild(parent, pt);
        if(!parent && !found)
            parent = new QTreeWidgetItem(this, QStringList() << pt);
        else if(!found)
            parent = new QTreeWidgetItem(parent, QStringList(pt));
        else
            parent = found;

        if(i < 2 && sl.size() > 4)
            parent->setExpanded(expand);
        if(i == sl.size() - 1)
            parent->setData(0, Qt::UserRole, name);
    }
}

void QuHdbBrowser::updateSourcesList(const QStringList& sources, bool expand)
{
    foreach(QString s, sources)
        addSource(s, expand);
}


TreeBuilder::TreeBuilder(const QStringList &srclist)
{
    srcs = srclist;
}

void TreeBuilder::run()
{

}
