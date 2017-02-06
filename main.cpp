#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <cassert>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    assert(argc==2 &&  "wrong number of arguments");
    string infilename = argv[1];
    
    MainWindow w(infilename);
    w.show();
    
    return 0;
}
