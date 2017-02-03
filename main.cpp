#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <cassert>
#include "board.h"

int breadth_first_search_dis(FArray2d<char> maze){
    Point endp = maze.dim()-Point(1,1);
    int enddis = -1;
    breadth_first_search(maze,Point(0,0),endp,[&](Point p,int dist){
        if(p == endp){
            enddis = dist;
        }
    });
    assert(enddis >= 0 && "bfs failed to find end");
    return enddis;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    assert(argc==2 &&  "wrong number of arguments");
    string infilename = argv[1];
    FArray2d<char> maze = make_maze(infilename);
    gen_image("original.png",maze);
    //add_line(maze,QPointF(5,100),QPointF(300,3));
    gen_image("cross1.png",maze);
    //add_line(maze,QPointF(50,30),QPointF(400,300));
    gen_image("cross2.png",maze);
    
    cout << breadth_first_search_dis(maze) << endl;
    
    return 0;
    //return a.exec();
}
