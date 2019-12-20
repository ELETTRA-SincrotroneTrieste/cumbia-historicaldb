#include "quhdbbrowser.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QuHdbBrowser w(nullptr);
    w.show();
    return a.exec();
}
