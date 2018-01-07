#pragma once
#include <QPoint>
#include <array>
#include <vector>
#include <iostream>
#include <RangeIterator.h>
using namespace std;

constexpr int bsize = 1000;
constexpr int init_rand_update = 10;
constexpr int max_depth = 16;
constexpr int maxlook = max_depth * 2 + 1;
template<typename numty>
inline numty Sq(numty x){
    return x * x;
}

inline double dist_func(int xdis,int ydis){
    int taxidis = abs(xdis) + abs(ydis);
    if(taxidis == 0){
        return 10e40;
    }
    else if(taxidis == 1 || taxidis == 2){
        return 5000;
    }
    else{
        return 3000.0 / (0.1+Sq(xdis*5) +  Sq(ydis*5));
    }
}
inline double cen_func(int x,int y){
    //gradient of F = c*(-x,-y)
    //should probably be:
    //c*(-x,-y) / ||(x,y)||
    return 3.0 * -sqrt(Sq(x) + Sq(y));
}
inline double depth_adj_fn(int depth){
    return ((depth)) / double(0.5*(max_depth));
}
inline double rand_fn(){
    return 5 * (rand() - RAND_MAX/2) / double(RAND_MAX/2);
}
struct point{
    int x;
    int y;
};
template<typename info_ty>
struct ValInfo{
    double val;
    info_ty info;
};
using valpoint = ValInfo<point>;

template<typename info_ty>
bool operator < (const ValInfo<info_ty> & one,const ValInfo<info_ty> & other){
    return one.val < other.val;
}
class point_obj{
public:
    int num_elmts = 0;
    QPointF cen = {0,0};
    
    bool filled[bsize][bsize];
    double spot_vals[bsize][bsize];
    double cum_rand_vals[bsize][bsize];
    static constexpr size_t valsize = maxlook+2;
    valpoint prevvals[max_depth][valsize][valsize];
    
    QPoint prevp = {bsize/2,bsize/2};
    
    vector<point> arr;
    
    point_obj(){
        memset(filled,0,bsize*bsize*sizeof(bool));
        memset(spot_vals,0,bsize*bsize*sizeof(double));
        memset(cum_rand_vals,0,bsize*bsize*sizeof(double));
        for(int i = 0; i < init_rand_update;i++){
            update_rand_vs();
        }
    }
    double cost_at(int depth,int x,int y){
        int realx = prevp.x() + x;
        int realy = prevp.y() + y;
        return (depth_adj_fn(depth)*spot_vals[realx][realy] + cen_func(realx-cen.x(),realy-cen.y()) + cum_rand_vals[realx][realy]);
    }
    valpoint & val_at(int depth,int x,int y){
        return prevvals[depth][x+max_depth+1][y+max_depth+1];
    }
    QPoint find_next(){
        for(int d : range(max_depth))
            for(int x : range(valsize))
                for(int y : range(valsize))
                    prevvals[d][x][y] = {-10e50,{0,0}};

        val_at(0,0,0) = {0,{0,0}};
        for(int d : range(1,max_depth)){
            for(int x : range(-max_depth,max_depth+1)){
                for(int y : range(-max_depth,max_depth+1)){
                    valpoint maxv = {-10e45,point{x,y}};
                    for(int i = x-1; i <= x+1; i++){
                        for(int j = y-1; j <= y+1; j++){
                            maxv = max(maxv,valpoint{val_at(d-1,i,j).val,point{i,j}});
                        }
                    }
                    val_at(d,x,y) = {maxv.val + cost_at(d,x,y),maxv.info};
                }
            }
        }
        ValInfo<point> maxv = {-10e20,point{0,0}};
        for(int x : range(-max_depth,max_depth+1)){
            for(int y : range(-max_depth,max_depth+1)){
                maxv = max(val_at(max_depth-1,x,y),maxv);
            }
        }
        for(int d : range(max_depth-2,1,-1)){
            maxv = val_at(d,maxv.info.x,maxv.info.y);
        }
        QPoint retv =  QPoint{maxv.info.x+prevp.x(),maxv.info.y+prevp.y()};
        if(cost_at(1,maxv.info.x,maxv.info.y) < -10e10){
            cout << maxv.info.x << endl << maxv.info.y << endl << cost_at(1,maxv.info.x,maxv.info.y) << endl;
            throw runtime_error("output too large");
        }
        if(abs(retv.x() - prevp.x()) > 1 || abs(retv.y() - prevp.y()) > 1){
            throw runtime_error("find_next output numbers too differnt");
        }
        if(retv.x() - prevp.x() == 0 && retv.y() - prevp.y() == 0){
            throw runtime_error("find_next output prevp");
        }
        return retv;
    }
    void add_next(QPoint p){
        prevp = p;
        
        cen *= num_elmts;
        cen += QPointF(p);
        num_elmts += 1;
        cen /= num_elmts;
        
        //undoes changes
        adj_spot_vals(p.x(),p.y());
        update_rand_vs();
    }
    void update_rand_vs(){
        for(int x = 0; x < bsize;x++){
            for(int y = 0; y < bsize;y++){
                cum_rand_vals[x][y] += rand_fn();
            }
        }
    }
    void adj_spot_vals(int avoidx,int avoidy){
        for(int x = 0; x < bsize;x++){
            for(int y = 0; y < bsize;y++){
                spot_vals[x][y] -= dist_func(x-avoidx,y-avoidy);
            }
        }
    }
};
