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
#include <cmath>
#include <random>
#include <ctime>
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
FArray2d<uint64_t> make_density(Point dim,const vector<Point> & points){
    FArray2d<uint64_t> res(dim.X,dim.Y);
    res.assign(0);
    for(Point p : points){
        res[p]++;
    }
    return res;
}

QImage gen_QImage_density(FArray2d<uint64_t> pointvals,QColor overall_col){
    Point dim = pointvals.dim();
    QImage img(dim.X,dim.Y,QImage::Format_ARGB32);
    
    uint64_t maxp = max(uint64_t(1),*max_element(pointvals.begin(),pointvals.end()));
    
    for(int y = 0; y < dim.Y; y++){
        for(int x = 0; x < dim.X; x++){
            double fullval =254.0; 
            double alpha = (fullval*pointvals[y][x]) / maxp;
            
            QColor col = overall_col;
            col.setAlpha(int(alpha));
            
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
            Point p(x,y);
            if(p != cen){
                fn(p);
            }
        }
    }
}
const Point null_pt = Point(-1,-1);
template<typename fn_ty>
void breadth_first_search(FArray2d<char> blocked_points,Point start,Point min_edge,Point max_edge,fn_ty fn){
    vector<Point> prevps;
    vector<Point> curps;
    
    int distance = 0;
    blocked_points[start] = true;
    curps.push_back(start);
    fn(null_pt,start,distance);
    do{
        prevps.swap(curps);
        curps.resize(0);
        
        for(Point pp : prevps){            
            iter_around(min_edge,max_edge,pp,[&](Point newp){
                if(!blocked_points[newp]){
                    blocked_points[newp] = true;
                    curps.push_back(newp);
                    fn(pp,newp,distance);
                }
            });
        }
        
        distance++;
    }while(curps.size());
}
template<typename max_fn_ty>
vector<Point> discrite_path_to_best(const FArray2d<char> & blocked_points,Point start,Point min_edge,Point max_edge,max_fn_ty max_fn){
    unordered_map<Point,Point> prev_ps;
    prev_ps.reserve(600);
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
template<typename max_fn_ty>
Point discrite_best(const FArray2d<char> & blocked_points,Point start,max_fn_ty max_fn){
    double maxval = -10e50;
    Point maxp = null_pt;
    
    Point min_edge = Point(0,0);
    Point max_edge = blocked_points.dim()-Point(1,1);
    
    iter_around(min_edge,max_edge,start,[&](Point newp){
        if(!blocked_points[newp]){
            double val = max_fn(newp);
            if(maxval < val){
                maxval = val;
                maxp = newp;
            }
        }
    });
    return maxp;
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
double rand_lin_val = 1.0;
double dest_lin_val = 0.0001;
double avoid_prev_lin_val = 0.3;
int walk_dis = 2;
string walk_name = "no-name";
int stride_sqrd(){return sqr(walk_dis)-1;}

struct loc_val{
    Point orig_cen;
    Point dest;
    QPointF cen;
    QPointF lin_vec;
    loc_val(Point incen,Point indest){
        orig_cen = incen;
        dest = indest;
        cen = q_pt(incen);
        lin_vec = QPointF(0,0);
    }
    void add_lin(Point p,double val){
        if(p != orig_cen){
            QPointF qp = q_pt(p);
            double dis = distance(qp,cen);
            double dis_adj_val = val / max(1.0,dis);
            lin_vec += (qp - cen) * dis_adj_val;
        }
    }
    double point_val(Point p){
        auto mag_vec = [](Point vec)->int{return sqr(vec.X)+sqr(vec.Y);};
        auto mag_vec_qp = [](QPointF vec)->double{return sqr(vec.x())+sqr(vec.y());};
        Point off = p - orig_cen;
        if(p == dest){
            return 10e20;
        }
        else if(mag_vec(off) <= stride_sqrd()){
            QPointF dif = q_pt(off) - lin_vec;
            double dis_to_lin = mag_vec_qp(dif);
            return dis_to_lin == 0 ? 10e50 : -dis_to_lin;
        }
        else{
            return -10000.0;
        }
    }
    double operator()(Point p){return point_val(p);}
};

QPointF rand_dir_point(QPointF cen){
    double theta = (rand()/double(RAND_MAX))*M_PI*2;
    double r = 1000.0;
    QPointF offset(r*cos(theta),r*sin(theta));
    return cen + offset;
}

//double avoid_val = 0;
//double lin_val = 0;
//double lin_val = 0;
loc_val gen_loc_val(Point cen,Point dest,Point prevp,Point back2p){
    loc_val lval(cen,dest);
    lval.add_lin(dest,dest_lin_val);
    lval.add_lin(prevp,avoid_prev_lin_val);
    lval.add_lin(back2p,avoid_prev_lin_val);
    lval.add_lin(my_point(rand_dir_point(q_pt(cen))),rand_lin_val);
    return lval;
}
template<typename bin_op_fn>
Point element_wise(Point one,Point other,bin_op_fn bop){
    return Point(bop(one.X,other.X),bop(one.Y,other.Y));
}
vector<Point> rand_liniar_walk(const FArray2d<char> & blocked_points,Point begin, Point end){
    vector<Point> res;
    Point back2p = begin;
    Point prevp = begin;
    Point curp = begin;
    int count = 1;
    while(curp != end){
        if(count % 1000000 == 0){
            //cout << count << endl;
            //cout << curp.X << ' ' << curp.Y << endl;
            //gen_QImage_density(make_density(blocked_points.dim(),res),Qt::green).save("test.png");
        }
        count++;
        loc_val lval = gen_loc_val(curp,end,prevp,back2p);
        
        Point nextp = discrite_best(blocked_points,curp,lval);
        
        res.push_back(nextp);
        
        back2p = prevp;
        prevp = curp;
        curp = nextp;
    }
    res.push_back(curp);
    return res;
}
FArray2d<uint64_t> rand_walk(const FArray2d<char> & blocked_points,Point begin, Point end){
    Point dim = blocked_points.dim();
    FArray2d<uint64_t> density(dim.X,dim.Y);
    density.assign(0);
    double dis = 0;
    
    Point minedge = Point(0,0);
    Point maxedge = blocked_points.dim()-Point(1,1); 
    
    Point curp = begin;
    vector<Point> avaliable_pts;
    
    default_random_engine gen(clock());
    
    avaliable_pts.reserve(8);
    while(curp != end){
        avaliable_pts.resize(0);
        iter_around(minedge,maxedge,curp,[&](Point p){
            if(!blocked_points[p]){
                avaliable_pts.push_back(p);
            }
        });
        uniform_int_distribution<int> dist(0,avaliable_pts.size()-1);
        int sample_val = dist(gen);
        Point randp = avaliable_pts[sample_val];
        
        density[curp]++;
        dis += distance(q_pt(randp),q_pt(curp));
        
        curp = randp;
        
        if((int(dis)+1)%100000000==0){
            gen_QImage_density(density,Qt::green).save("rtest.png");
            //break;
        }
    }
    cout << dis << endl;
    return density;
}


vector<QPointF> conv_vec(const vector<Point>  & points){
    vector<QPointF> res(points.size());
    for(size_t i = 0; i < points.size(); i++){
        res[i] = middle_of(points[i]);
    }
    return res;
}
double connected_distance(const vector<QPointF> & points){
    double sum = 0;
    for(size_t i = 1; i < points.size(); i++){
        sum += distance(points[i-1],points[i]);
    }
    return sum;
}

void add_line(FArray2d<char> & maze,QPointF p1,QPointF p2){
    iter_points_between(p1,p2,[&](Point p){
        maze[p] = !maze[p];
    });
}

