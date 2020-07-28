#include "Router.h"

#include <QApplication>
#include <iostream>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // permet la traduction de certains éléments de qt
    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&translator);

    Router w;
    w.setWindowTitle("Teeko");
    w.show();
    return a.exec();
}
