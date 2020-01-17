#include "quhdbexplorer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QuHdbExplorer w(nullptr);
    w.show();
    return a.exec();
}
