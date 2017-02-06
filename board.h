#pragma once
#include <QPoint>
#include <array>
#include <vector>
#include <QPixmap>
#include <QImage>
#include <cassert>
#include <QRgb>
#include <unordered_set>
#include <unordered_map>
#include <cstdlib>
#include "point.h"
#include "farray2d.h"
#include "walk.h"
using namespace std;

bool is_white(QRgb col){
    return qRed(col) + qGreen(col) + qBlue(col) > (255*3)/2;
}
QImage gen_QImage_Maze(FArray2d<char> blocked_points){
    Point dim = blocked_points.dim();
    QImage img(dim.X,dim.Y,QImage::Format_ARGB32);
    for(int y = 0; y < dim.Y; y++){
        for(int x = 0; x < dim.X; x++){
            QColor col = (blocked_points[y][x]==0)?
                Qt::white :
                Qt::black;
            
            img.setPixelColor(x,y,col);
            
        }
    }
    return img;
}

FArray2d<int> make_density(Point dim,const vector<Point> & points){
    FArray2d<int> res(dim.X,dim.Y);
    res.assign(0);
    for(Point p : points){
        res[p]++;
    }
    return res;
}

QImage gen_QImage_density(FArray2d<int> pointvals){
    Point dim = pointvals.dim();
    QImage img(dim.X,dim.Y,QImage::Format_ARGB32);
    
    int maxp = *max_element(pointvals.begin(),pointvals.end());
    
    for(int y = 0; y < dim.Y; y++){
        for(int x = 0; x < dim.X; x++){
            int alpha = (255*pointvals[y][x]) / maxp;
            QColor col(0,255,0,alpha);
            
            img.setPixelColor(x,y,col);
            
        }
    }
    return img;
}

void gen_image(QString outname,FArray2d<char> blocked_points){
    gen_QImage_Maze(blocked_points).save(outname);
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
const Point null_pt = Point(-1,-1);
template<typename fn_ty>
void breadth_first_search(const FArray2d<char> & blocked_points,Point start,Point min_edge,Point max_edge,fn_ty fn){
    vector<Point> prevps;
    vector<Point> curps;
    unordered_set<Point> accessed_pts;
    
    auto add_point = [&](Point prevp,Point newp,int dis){
        accessed_pts.insert(newp);
        curps.push_back(newp);
        fn(prevp,newp,dis);
    };
    auto point_valid = [&](Point p){
        return !accessed_pts.count(p) && !blocked_points[p];
    };
    
    int distance = 0;
    add_point(null_pt,start,distance);
    do{
        prevps.swap(curps);
        curps.resize(0);
        
        for(Point pp : prevps){            
            iter_around(min_edge,max_edge,pp,[&](Point newp){
                if(point_valid(newp)){
                    add_point(pp,newp,distance);
                }
            });
        }
        
        distance++;
    }while(curps.size());
}
template<typename max_fn_ty>
vector<Point> discrite_path_to_best(const FArray2d<char> & blocked_points,Point start,Point min_edge,Point max_edge,max_fn_ty max_fn){
    unordered_map<Point,Point> prev_ps;
    double maxval = -10e50;
    Point maxp = null_pt;
    
    breadth_first_search(blocked_points,start,min_edge,max_edge,[&](Point prevp,Point newp,int ){
        double val = max_fn(newp);
        prev_ps[newp] = prevp;
        if(maxval < val){
            maxval = val;
            maxp = newp;
        }
    });
    vector<Point> back_track;
    Point curp = maxp;
    while(curp != null_pt){
        back_track.push_back(curp);
        curp = prev_ps[curp];
    }
    vector<Point> forward_path(back_track.rbegin(),back_track.rend());
    return forward_path;
}

vector<QPointF> continuous_path(const FArray2d<char> & blocked_points,const vector<Point> & path){
    vector<QPointF> res;
    size_t startidx = 0;
    res.push_back(middle_of(path[startidx]));
    for(size_t i = 1; i < path.size(); i++){
        QPointF start = middle_of(path[startidx]);
        QPointF curp = middle_of(path[i]);
        if(i > startidx + 1 && !direct_path_clear(blocked_points,curp,start)){
            res.push_back(start);
            startidx = i - 1;
        }
    } 
    if(my_point(res.back()) != path.back()){
        res.push_back(middle_of(path.back()));
    }
    return res;
}

vector<Point> rand_walk(const FArray2d<char> & blocked_points,Point begin, Point end){
    Point minedge = Point(0,0);
    Point maxedge = blocked_points.dim()-Point(1,1); 
    
    vector<Point> res;
    Point curp = begin;
    vector<Point> avaliable_pts;
    avaliable_pts.reserve(8);
    while(curp != end){
        avaliable_pts.resize(0);
        res.push_back(curp);
        iter_around(minedge,maxedge,curp,[&](Point p){
            if(!blocked_points[p]){
                avaliable_pts.push_back(p);
            }
        });
        Point randp = avaliable_pts[rand()%avaliable_pts.size()];
        curp = randp;
    }
    res.push_back(curp);
    return res;
}
vector<QPointF> conv_vec(const vector<Point>  & points){
    vector<QPointF> res(points.size());
    for(int i = 0; i < points.size(); i++){
        res[i] = middle_of(points[i]);
    }
    return res;
}

void add_line(FArray2d<char> & maze,QPointF p1,QPointF p2){
    iter_points_between(p1,p2,[&](Point p){
        maze[p] = !maze[p];
    });
}

