#include "profile.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QRegularExpression>
#include <cumacros.h>
#include <QDir>
#include <QDateTime>
#include <cumbiahdbworld.h>
#include "../../src/cuhdb_config.h"

Option::Option()
{
    default_option_idx = -1;
    optionType = None;
}

bool Option::isEmpty() const
{
    return value.isEmpty();
}

Profile::Profile(const QString &name) {
    // default value for name: "" (see declaration)
    m_name = name;
}

void Profile::setName(const QString &nam)
{
    m_name = nam;
}

QString Profile::name() const {
    return m_name;
}

bool Profile::load(const QString &name, bool is_template) {
    m_optionsList.clear();
    m_map.clear();
    m_name = name;
    QString fpath;
    is_template ? fpath = name : fpath = profilePath(name);
    m_errMsg.clear();
    QFile file(fpath);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString desc, key;
        QStringList options;
        QTextStream in(&file);
        while(!in.atEnd() && m_errMsg.isEmpty()) {
            QString l = in.readLine();
            if(l.contains(QRegularExpression("#\\s*"))) {
                l.remove(QRegularExpression("#\\s*"));
                if(!l.isEmpty())
                    desc += l + "\n";
            }
            else {
                // we find a key = value line
                // save desc and process
                QRegularExpression re("([A-Za-z0-9]+)\\s*=\\s*(.*)");
                QRegularExpressionMatch ma = re.match(l);
                if(ma.hasMatch()) {
                    key = ma.captured(1).trimmed();
                    Option opt;
                    opt.optionDesc = desc;
                    desc.clear(); // clear desc buffer
                    if(ma.capturedTexts().size() > 2) {
                        opt.value = ma.captured(2).trimmed();
                        if(opt.value.isEmpty()) {
                            m_errMsg = "Profile.load: value associated to \"" + key + "\" cannot be empty";
                        }
                        if(!opt.value.contains(QRegularExpression("%[sdf]"))) {
                            options = opt.value.split(QRegularExpression("\\s*,\\s*"),
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
                                                      Qt::SkipEmptyParts);
#else
                                                      QString::SkipEmptyParts);
#endif
                            for(int i = 0; i < options.size(); i++) {
                                QString o = options[i];
                                if(o.endsWith('*')) {
                                    opt.default_option_idx = i;
                                    o.remove("*");
                                }
                                opt.options << o;
                                opt.optionType = Option::Enum;
                            }
                        }
                        else {
                            if(opt.value.contains("%s"))
                                opt.optionType = Option::String;
                            else if(opt.value.contains("%d"))
                                opt.optionType = Option::Integer;
                            else if(opt.value.contains("%f"))
                                opt.optionType = Option::Float;
                        }
                    }
                    m_map[key] = opt;
                    if(!m_optionsList.contains(key))
                        m_optionsList << key;
                } // re.indexIn() >= 0
            }
        } // while !in.atEnd
        file.close();
    }
    else {
        m_errMsg = QString("hdb-db-profile-manager: Profile.load failed to open template file \"%1\"").arg(fpath);
    }
    return m_errMsg.isEmpty();
}

bool Profile::isEmpty() const {
    return m_map.isEmpty();
}

bool Profile::hasError() const {
    return m_errMsg.size() > 0;
}

Option Profile::getOption(const QString &key)
{
    if(m_map.contains(key))
        return m_map[key];
    return Option();
}

QStringList Profile::getOptionkeys() const
{
    return m_optionsList;
}

QString Profile::valueAsStr(const QString &key) const
{
    if(m_map.contains(key))
        return m_map[key].value;
    return "";
}

Option::OptionType Profile::getType(const QString &key) const
{
    if(m_map.contains(key))
        return m_map[key].optionType;
    return Option::None;
}

QStringList Profile::getOptions(const QString &key)
{
    if(m_map.contains(key)) {
        return m_map[key].options;
    }
    return QStringList();
}

bool Profile::hasKey(const QString& key) const
{
    return m_optionsList.contains(key);
}

void Profile::set(const QString &key, const QVariant &value)
{
    Option o;
    m_map[key] = o;
    m_map[key].value = value.toString();
    if(!m_optionsList.contains(key))
        m_optionsList << key; // stores the ordered keys
}

bool Profile::save()
{
    m_name.isEmpty() ? m_errMsg = "Profile.save: cannot save a profile with an empty name" : m_errMsg = "";
    if(m_name.isEmpty())
        return false;
    QString dirpath = QString::fromStdString(CumbiaHdbWorld().getDbProfilesDir());
    QDir profiles_dir(dirpath);
    bool ok = profiles_dir.exists();
    if(!ok)
        ok = profiles_dir.mkpath(dirpath);
    if(!ok)
        m_errMsg = QString("hdb-db-profile-manager: Profile.save: failed to create directory \"%1\"").arg(dirpath);

    QString fpath = QString("%1/%2.%3").arg(dirpath).arg(m_name).arg(PROFILES_EXTENSION);
    QFile file(fpath);
    ok = file.open(QIODevice::WriteOnly|QIODevice::Text);
    if(ok) {
        QTextStream out(&file);
        out << "# generated with hdb-db-profile-manager\n";
        out << "# " <<  QDateTime::currentDateTime().toString() + "\n";
        foreach(QString s, m_optionsList) {
            const Option &o = m_map[s];
            out << s << " = " << o.value << "\n";
        }
        // see how many profiles are currently defined
        if(profiles_dir.entryInfoList(QStringList() << QString("*.%1").arg(PROFILES_EXTENSION)).size() == 1) { // it becomes the default
            setDefault(m_name);
        }
        file.close();
    }
    else {
        m_errMsg = QString("hdb-db-profile-manager: Profile.save failed to open output file \"%1\"").arg(fpath);
    }
    return ok;
}

QString Profile::errorMessage() const {
    return m_errMsg;
}

bool Profile::deleteProfile(const QString &name)
{
    m_errMsg.clear();
    QString dirpath = QString::fromStdString(CumbiaHdbWorld().getDbProfilesDir());
    QDir profiles_dir(dirpath);
    QString fpath = profilePath(name);
    QFile file(fpath);
    bool ok = file.exists();
    if(ok) {
        ok = file.remove();
        if(ok) { // remove symlink if points to name
            fpath = defaultLinkPath();
            QFile lnk(fpath);
            QFileInfo fi(lnk);
            if(fi.exists() && fi.isSymLink() && fi.symLinkTarget() == fpath) {
                ok = lnk.remove();
            }
            // only one profile remains? make it the default one
            QStringList filter = QStringList() << QString("*.%1").arg(PROFILES_EXTENSION);
            if(profiles_dir.entryInfoList(filter).size() == 1) { // it becomes the default
                QString lone_profile = profiles_dir.entryInfoList(filter).first().fileName();
                ok = setDefault(lone_profile);
            }
        }
    }
    else
        m_errMsg = QString("hdb-db-profile-manager: Profile.deleteProfile failed to remove file \"%1\"").arg(fpath);
    return ok;
}

QStringList Profile::list(QString& default_prof) const
{
    QStringList prlist;
    QString default_profile;
    QFileInfoList li;
    QDir profiles_dir(QString::fromStdString(CumbiaHdbWorld().getDbProfilesDir()));
    if(profiles_dir.exists()) {
        QString ext = QString(".%1").arg(PROFILES_EXTENSION);
        QFile lnk_default(profiles_dir.absoluteFilePath(defaultLinkNam()));
        if(lnk_default.exists() && QFileInfo(lnk_default).isSymLink())
            default_profile = QFileInfo(lnk_default).symLinkTarget();
        li = profiles_dir.entryInfoList(QStringList() << "*" + ext);
        for(int i = 0; i < li.size(); i++) {
            QString fnam = li[i].fileName();
            if(fnam != defaultLinkNam() && fnam.endsWith(ext)) {
                fnam.remove(ext);
                prlist << fnam;
            }
            if(li[i].absoluteFilePath() == default_profile)
                default_prof = fnam;
        }
    }
    return prlist;
}

bool Profile::setDefault(const QString &profile)
{
    m_errMsg.clear();
    QString fpath = profilePath(profile);
    QFile file(fpath);
    bool ok = file.exists();
    if(ok) {
        fpath = defaultLinkPath();
        QFile lnk(fpath);
        QFileInfo fi(lnk);
        if(fi.exists() && fi.isSymLink())
            ok = lnk.remove();
        if(ok) {
            ok = file.link(fpath);
            if(!ok)
                m_errMsg = "Profile.setDefault: failed to create symlink to " + fpath;
            else
                printf("\e[1;32m*\e[0m %s --> \e[0;32mdefault\e[0m\n", qstoc(profile));
        }
        else {
            m_errMsg = "Profile.setDefault: failed to remove old link";
        }
    }
    else
        m_errMsg = "Profile: profile \"" + profile + "\" does not exist";

    return m_errMsg.isEmpty();
}

QString Profile::defaultLinkNam() const {
    return QString("%1.%2").arg(PROFILE_DEFAULT_NAME).arg(PROFILES_EXTENSION);
}

QString Profile::defaultLinkPath() const
{
    return QString("%1/%2").arg(QString::fromStdString(CumbiaHdbWorld().getDbProfilesDir())).arg(defaultLinkNam());
}

QString Profile::profilePath(const QString& profile_name) const
{
    return QString("%1/%2.%3").arg(QString::fromStdString(CumbiaHdbWorld().getDbProfilesDir()))
        .arg(profile_name).arg(PROFILES_EXTENSION);
}
