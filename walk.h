#pragma once
#include <vector>
#include <cassert>
#include <QPointF>
#include "point.h"
#include "farray2d.h"
using namespace std;


double sqr(double x){
    return x*x;
}

double distance(QPointF p1,QPointF p2){
    return sqrt(QPointF::dotProduct(p1,p2));
}
QPoint q_pt(Point p){
    return QPoint(p.X,p.Y);
}
Point my_point(QPointF p){
    return Point{int(p.x()),int(p.y())};
}
Point point_dir(QPointF fdir){
    int xsign = fdir.x() > 0 ? 1 : -1;
    int ysign = fdir.y() > 0 ? 1 : -1;
    return Point(xsign,ysign);
}
double slope(QPointF s,QPointF e){
    double dx = e.x() - s.x();
    double dy = e.y() - s.y();
    return dx == 0 ? 10e20*dy : dy/dx;
}
double intercept(QPointF p,double slope){
    return p.y() - p.x() * slope;
}
struct line_int{
    qreal m;
    qreal b;
    
    qreal minx;
    qreal miny;
    qreal maxx;
    qreal maxy;
    line_int(QPointF s,QPointF e){
        m = slope(s,e);
        b = intercept(s,m);
        
        minx = min(s.x(),e.x());
        miny = min(s.y(),e.y());
        
        maxx = max(s.x(),e.x());
        maxy = max(s.y(),e.y());
    }
    bool inside_line_bounds(QPointF p)const{
        return p.x() > minx*0.99999 && p.x() < maxx*1.00001 &&
               p.y() > miny*0.99999 && p.y() < maxy*1.00001;
    }
};

bool intersects(const line_int & l1,const line_int & l2){
    qreal xint = (l2.b-l1.b)/(l1.m-l2.m);
    qreal yint = l1.m*xint+l1.b;
    QPointF intp(xint,yint);
    return l1.inside_line_bounds(intp) && l2.inside_line_bounds(intp);
}

template<typename fn_ty>
void iter_points_between(QPointF start,QPointF end,fn_ty itfn){
    const Point startp = my_point(start);
    const Point endp = my_point(end);
    const Point tot_dir = point_dir(end-start);
    const Point xdir = Point(tot_dir.X,0);
    const Point ydir = Point(0,tot_dir.Y);
    const QPointF xdirf = q_pt(xdir);
    const QPointF ydirf = q_pt(ydir);
    const line_int moveline(start,end);
    
    
    Point curp = startp;
    while(curp != endp){
        itfn(curp);
        QPointF curpf = q_pt(curp);
        
        const line_int edge1(curpf,curpf-xdirf);
        const line_int edge2(curpf,curpf-ydirf);
        if(intersects(moveline,edge1)){
            curp = curp + ydir;
        }
        else if(intersects(moveline, edge2)){
            curp = curp + xdir;
        }
        else{
            assert(false && "intersects with neither line");
        }
    }
    itfn(endp);
}

vector<Point> walk_maze(FArray2d<char> & maze,double stridelen){
    vector<Point> points;
    const QPointF end = q_pt(maze.dim()-Point(1,1));
    QPointF curp(0,0);
    
    while(true){
        //i
    }
    return points;
}
