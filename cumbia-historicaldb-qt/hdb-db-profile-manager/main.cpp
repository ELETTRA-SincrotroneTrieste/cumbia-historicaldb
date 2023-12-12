#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <cumacros.h>
#include <QString>
#include <QtDebug>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include "profile.h"

#define TEMPLATE_FILENAM "dbtemplate.dat"

int main(int argc, char *argv[])
{
    bool ok = true;
    QCoreApplication app(argc, argv);
    QTextStream txtin(stdin);
    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption addProfile(QStringList() << "a" << "add-profile",
                                  "add a new profile", "profile");
    parser.addOption(addProfile);
    QCommandLineOption editProfile(QStringList() << "e" << "edit-profile",
                                   "edit an existing profile", "profile");
    parser.addOption(editProfile);
    QCommandLineOption delProfile(QStringList() << "r" << "remove-profile",
                                  "remove a profile", "profile");
    parser.addOption(delProfile);

    QCommandLineOption setDefault(QStringList() << "d" << "set-default",
                                  "set the default profile", "profile");
    parser.addOption(setDefault);


    QCommandLineOption listProfiles(QStringList() << "l" << "list-profiles",
                                    "list profiles");
    parser.addOption(listProfiles);

    parser.addPositionalArgument("profile", "show profile");
    parser.process(app);

    Profile out_profile, template_profile; // edit profile and template profile
    if(parser.isSet(addProfile)) { // do not touch ok
        if(out_profile.load(parser.value(addProfile)))
            printf("\e[1;33m*\e[0m warning: editing existing profile \"%s\"\n", qstoc(out_profile.name()));
    }
    else if(parser.isSet(editProfile)) {
        ok = out_profile.load(parser.value(editProfile));
        if(!ok)
            perr("error loading profile \"%s\": %s", qPrintable(out_profile.name()), qPrintable(out_profile.errorMessage()));
    }
    if(parser.isSet(addProfile) || parser.isSet(editProfile))
    {
        if(ok) {
            if(!template_profile.load(QString(":%1").arg(TEMPLATE_FILENAM), true)) {
                perr("hdb-db-profile-manager: error loading template \"%s\": %s", TEMPLATE_FILENAM,
                     qstoc(template_profile.errorMessage()));
            }
            else {
                foreach(QString key, template_profile.getOptionkeys()) {
                    QString value, edit_value;
                    int choice = 0;
                    char type[16];
                    const Option &o = template_profile.getOption(key);
                    printf("%s", qstoc(o.optionDesc));
                    if(out_profile.hasKey(key)) {
                        edit_value = out_profile.valueAsStr(key);
                    }
                    strncpy(type, o.optionNames[o.optionType], 15);
                    printf("\e[1;32m%s\e[0m = ", qstoc(key));
                    if(!edit_value.isEmpty())
                        printf(" [ \e[1;34m%s\e[0m ]: ", qstoc(edit_value));

                    if(o.optionType == Option::Enum) {
                        // print options
                        for(int i = 0; i < o.options.size(); i++) {
                            QString op = o.options[i];
                            i == o.default_option_idx ? printf("%s [%d] [default]\n", qstoc(op.trimmed()), i+1) :
                                                        printf("%s [%d]\n", qstoc(op.trimmed()), i+1);
                        }
                        printf("[ %s ] > ", type);
                        choice = txtin.readLine().toInt(&ok) - 1;
                        ok = ok && choice >= 0 && choice < o.options.size();
                        if(!ok)
                            choice = -1;
                        if(choice > -1) value = o.options[choice];
                        else if(!edit_value.isEmpty()) value = edit_value;
                        else value = o.options[o.default_option_idx];
                        printf("-set value %s for enum\n", qstoc(value));
                    }
                    else if(o.optionType == Option::Integer) {
                        do {
                            printf("[ %s ] > ", type);
                            value = txtin.readLine();
                            value.toInt(&ok);
                        } while(!ok && edit_value.isEmpty());
                    }
                    else if(o.optionType == Option::Float) {
                        do {
                            printf("[ %s ] > ", type);
                            value = txtin.readLine();
                            value.toDouble(&ok);
                        } while(!ok && edit_value.isEmpty());
                    }
                    else if(o.optionType == Option::String) {
                        do {
                            printf("[ %s ] > ", type);
                            value = txtin.readLine();
                            ok = !value.isEmpty();
                        } while(!ok && edit_value.isEmpty());
                    }
                    !value.isEmpty() ? out_profile.set(key, value) : out_profile.set(key, edit_value);
                } // foreach(QString key, template_profile.getOptionkeys
            } // ok loading template

            bool saved = out_profile.save();
            if(!saved) {
                perr("hdb-db-profile-manager: failed to save profile: \"%s\": %s",
                     qstoc(parser.value(editProfile)), qstoc(out_profile.errorMessage()));
            }
            else
                printf("\e[1;32m*\e[0m saved profile \e[0;3m%s\e[0m\n", qstoc(parser.value(editProfile)));
        } // if ok
    } // parser.isSet( edit or new profile)
    else if(parser.isSet(delProfile)) {
        Profile f;
        bool ok = f.deleteProfile(parser.value(delProfile));
        if(!ok)
            perr("failed to remove profile \"%s\": %s\n", qstoc(parser.value(delProfile)), qstoc(f.errorMessage()));
    }
    else if(parser.isSet(listProfiles)) {
        Profile f;
        QString default_profilenam;
        QStringList list = f.list(default_profilenam);
        foreach(QString p, list) {
            p == default_profilenam ? printf("%s [\e[1;32m default\e[0m ]\n", qPrintable(p))
                                    : printf("%s\n", qPrintable(p));
        }
    }
    else if(parser.positionalArguments().size() > 0) {
        QString pnam = parser.positionalArguments().first();
        Profile p;
        p.load(pnam);
        if(!p.isEmpty()) {
            printf("\nProfile \e[0;32;4m%s\e[0m\n\n", qPrintable(pnam));
            foreach(QString k, p.getOptionkeys()) {
                printf("%s: \e[0;3m%s\e[0m\n", qPrintable(k), qPrintable(p.valueAsStr(k)));
            }
        }
        else
            printf("\e[1;33m*\e[0m no such profile \"%s\"\n", qPrintable(pnam));
    }
    else if(parser.isSet(setDefault)) {
        ok = out_profile.setDefault(parser.value(setDefault));
        if(!ok)
            perr("error setting default profile \"%s\": %s", qPrintable(parser.value(setDefault)),
                 qstoc(out_profile.errorMessage()));
    }
    else
        printf("%s\n", qPrintable(parser.helpText()));

}
