#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[])
{
    srand(clock());
    QApplication a(argc, argv);
    
    assert(argc==6 &&  "wrong number of arguments");
    string infilename = argv[1];
    rand_lin_val = stod(string(argv[2]));
    dest_lin_val  =  stod(string(argv[3]));
    avoid_prev_lin_val  =  stod(string(argv[4]));
    walk_dis =  stoi(string(argv[5]));
    cout << rand_lin_val << "\t"
         << dest_lin_val << "\t" 
         << avoid_prev_lin_val << "\t" 
         << walk_dis << "\t";
            
    MainWindow w(infilename);
    w.show();
    
    return 0;
}
