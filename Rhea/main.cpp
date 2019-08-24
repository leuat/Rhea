#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if (argc!=1) {
//        qDebug() << "Argc " << argc;
        QString file = argv[1];
        w.m_computer.m_cpu.LoadProgram(file);
    }
    else {
        w.m_computer.m_cpu.LoadProgram("/home/leuat/Dropbox/TRSE/Rhea/test1.prg");
    }
    w.show();

    return a.exec();
}
