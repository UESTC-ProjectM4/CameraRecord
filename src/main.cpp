#include <QtGui>
#include "widget.h"

int main(int argc,char *argv[])
{
    QApplication app(argc,argv);
    ProcessImage process;
    process.resize(480,240);
    //process.resize(960,480);
    process.show();

    return app.exec();
}



//#include <QtGui/QApplication>
//#include "widget.h"
//
//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    Widget w;
//    w.show();
//    return a.exec();
//}
