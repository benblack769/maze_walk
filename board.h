#pragma once
#include <QPoint>
#include <array>
#include <vector>
#include <QPixmap>
#include <QImage>
#include <cassert>
#include <QRgb>
#include "point.h"
#include "farray2d.h"
#include "walk.h"
using namespace std;

bool is_white(QRgb col){
    return qRed(col) + qGreen(col) + qBlue(col) > (255*3)/2;
}

void gen_image(QString outname,FArray2d<char> blocked_points){
    Point dim = blocked_points.dim();
    QImage img(dim.X,dim.Y,QImage::Format_ARGB32);
    for(int y = 0; y < dim.Y; y++){
        for(int x = 0; x < dim.X; x++){
            QColor col = (blocked_points[y][x]==0)?
                Qt::black :
                Qt::white;
            
            img.setPixelColor(x,y,col);
            
        }
    }
    img.save(outname);
}

FArray2d<char> make_maze(string image_file){
    QString fname = QString::fromStdString(image_file);
    QPixmap piximg(fname);
    QImage img = piximg.toImage();
    QSize size = img.size();
    FArray2d<char> res(size.width(),size.height());
    for(int y = 0; y < size.height(); y++){
        for(int x = 0; x < size.width(); x++){
            QRgb col = img.pixel(x,y);
            res[y][x] = !is_white(col);
        }
    }
    return res;
}
template<typename fn_ty>
void iter_around(Point min_edge,Point max_edge,Point cen,fn_ty fn){
    for(int y = max(cen.Y-1,min_edge.Y); y <= min(cen.Y+1,max_edge.Y); y++){
        for(int x = max(cen.X-1,min_edge.X); x <= min(cen.X+1,max_edge.X); x++){
            fn(Point(x,y));
        }
    }
}
template<typename fn_ty>
void breadth_first_search(FArray2d<char> blocked_points,Point min_edge,Point max_edge,fn_ty fn){
    Point boarddim = blocked_points.dim();
    Point fin_point = boarddim - Point(1,1);
    vector<Point> prevps;
    Point start = Point(0,0);
    prevps.push_back(start);
    blocked_points[start] = true;
    
    int distance = 0;
    while(prevps.size()){
        vector<Point> curps;
        
        for(Point pp : prevps){
            
            fn(pp,distance);
            
            iter_around(min_edge,max_edge,pp,[&](Point newp){
                if(!blocked_points[newp]){
                    blocked_points[newp] = true;
                    curps.push_back(newp);
                }
            });
        }
        
        distance++;
        prevps.swap(curps);
    }
}
void add_line(FArray2d<char> & maze,QPointF p1,QPointF p2){
    iter_points_between(p1,p2,[&](Point p){
        maze[p] = !maze[p];
    });
}
