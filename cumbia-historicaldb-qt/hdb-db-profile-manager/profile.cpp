#include "profile.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QRegExp>
#include <cumacros.h>
#include <QDir>
#include <QDateTime>
#include <cumbiahdbworld.h>

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
    CumbiaHdbWorld wo;
    m_optionsList.clear();
    m_map.clear();
    m_name = name;
    QString fpath;
    is_template ? fpath = name : fpath = QString("%1/%2.dat").arg(wo.getDbProfilesDir().c_str()).arg(name);
    m_errMsg.clear();
    QFile file(fpath);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString desc, key;
        QStringList options;
        QTextStream in(&file);
        while(!in.atEnd() && m_errMsg.isEmpty()) {
            QString l = in.readLine();
            if(l.contains(QRegExp("#\\s*"))) {
                l.remove(QRegExp("#\\s*"));
                if(!l.isEmpty())
                    desc += l + "\n";
            }
            else {
                // we find a key = value line
                // save desc and process
                QRegExp re("([A-Za-z0-9]+)\\s*=\\s*(.*)");
                if(re.indexIn(l) >= 0) {
                    key = re.capturedTexts()[1].trimmed();
                    Option opt;
                    opt.optionDesc = desc;
                    desc.clear(); // clear desc buffer
                    if(re.capturedTexts().size() > 2) {
                        opt.value = re.capturedTexts()[2].trimmed();
                        if(opt.value.isEmpty()) {
                            m_errMsg = "Profile.load: value associated to \"" + key + "\" cannot be empty";
                        }
                        if(!opt.value.contains(QRegExp("%[sdf]"))) {
                            options = opt.value.split(QRegExp("\\s*,\\s*"), QString::SkipEmptyParts);
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

    QString fpath = QString("%1/%2.dat").arg(dirpath).arg(m_name);
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
    QString fpath = QString("%1/%2.dat").arg(QString::fromStdString(CumbiaHdbWorld().getDbProfilesDir())).arg(name);
    QFile file(fpath);
    bool ok = file.exists();
    if(ok)
        ok = file.remove();
    if(!ok)
        m_errMsg = QString("hdb-db-profile-manager: Profile.deleteProfile failed to remove file \"%1\"").arg(fpath);
    return ok;
}

QStringList Profile::list() const
{
    QStringList li;
    QDir profiles_dir(QString::fromStdString(CumbiaHdbWorld().getDbProfilesDir()));
    if(profiles_dir.exists()) {
        li = profiles_dir.entryList(QStringList() << "*.dat");
        for(int i = 0; i < li.size(); i++)
            li[i].remove(".dat");
    }
    return li;
}
