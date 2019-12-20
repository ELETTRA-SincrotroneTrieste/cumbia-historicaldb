#ifndef PROFILE_H
#define PROFILE_H
#include <QStringList>
#include <QMap>
#include <QVariant>

class Option {
public:
    Option();

    enum OptionType { None, String, Float, Integer, Enum };
    QStringList options;
    int default_option_idx;
    OptionType optionType;
    QString optionDesc;
    QString value;
    bool isEmpty() const;
    const char *optionNames[16] = { "none", "string", "float", "int", "enum" };
};

class Profile
{
public:
    Profile(const QString& name = QString());
    void setName(const QString& nam);
    QString name() const;
    bool load(const QString& name, bool is_template = false);
    bool isEmpty() const;
    bool hasError() const;
    Option getOption(const QString& key);
    QStringList getOptionkeys() const;
    QString valueAsStr(const QString& key) const;
    Option::OptionType getType(const QString& key) const;
    QStringList getOptions(const QString& key);
    bool hasKey(const QString &key) const;
    void set(const QString &key, const QVariant &value);
    bool save();
    QString errorMessage() const;
    bool deleteProfile(const QString& name);
    QStringList list() const;

private:
    QStringList m_optionsList; // store the options order
    QMap<QString, Option> m_map;
    QString m_errMsg, m_name;
};

#endif // PROFILE_H
