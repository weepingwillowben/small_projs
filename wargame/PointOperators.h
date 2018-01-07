#pragma once
#include "stdafx.h"
struct Point{//these need to be int32 for addition reasons
    int32_t X;
    int32_t Y;
};
inline Point CreatePoint(int X, int Y){
    /*Point P;
    P.X = X;
    P.Y = Y;*/
    return{ X, Y };
}
inline Point ClearPoint(){
    return CreatePoint(-1,-1);
}
inline Point abs(Point P){
    return CreatePoint(abs(P.X), abs(P.Y));
}
inline bool operator==(Point P1, Point P2){
    return P1.X == P2.X && P1.Y == P2.Y;
}
inline bool operator!=(Point P1, Point P2){
    return !(P1 == P2);
}
inline void operator += (Point P1, Point P2){
    P1.X += P2.X;
    P1.Y += P2.Y;
}
template <typename NumType>
inline Point operator * (Point P1, NumType Mult){//do not make Point a refrence!!!
    //P1 is copied, so the *= do not affect the original
    P1.X *= Mult;
    P1.Y *= Mult;
    return P1;
}
inline Point operator + (Point P1, Point P2){//do not make Point a refrence!!!
    //P1 is copied, so the += do not affect the original
    P1 += P2;
    return P1;
}
template <typename NumType>
inline void operator /= (Point P1, NumType Num){
    P1.X /= Num;
    P1.Y /= Num;
}
inline Point operator - (Point P){//do not refrence!
    P.X = -P.X;
    P.Y = -P.Y;
    return P;
}
inline void operator -= (Point P1, Point P2){
    P1.X -= P2.X;
    P1.Y -= P2.Y;
}
inline Point operator - (Point P1, Point P2){//do not refrence!
    P1 -= P2;
    return P1;
}
class PointIterator{
public:
    PointIterator(Point Center, int Range){
        XCap = min(Center.X + Range + 1,BoardSizeX);
        YCap = min(Center.Y + Range + 1, BoardSizeY);

        YLow = max(Center.Y - Range, 0);

        P.X = max(Center.X - Range, 0);
        P.Y = YLow;
    }
    PointIterator(int xstart, int ystart, int xend, int yend){
        XCap = min(xend + 1,BoardSizeX);
        YCap = min(yend + 1,BoardSizeY);

        P.X = max(xstart,0);
        P.Y = max(ystart,0);

        YLow = P.Y;
    }
    PointIterator(){
        XCap = BoardSizeX;
        YCap = BoardSizeY;

        YLow = 0;
        P.X = 0;
        P.Y = 0;
    }
    bool NotEnd(){
        return P.X < XCap;
    }
    bool operator != (PointIterator & Other){
        return P.X < Other.XCap;
    }

    void operator++ (){
        P.Y++;
        if (P.Y >= YCap){
            P.X++;
            P.Y = YLow;
        }
    }
    Point operator * (){
        return P;
    }
private:
    Point P;
    int XCap,YCap,YLow;
};
class PIterContainter
{
public:
    PointIterator EndIter,StartIter;
    PIterContainter(Point Center, int Range){
        EndIter = PointIterator(Center, Range);
        StartIter = EndIter;
    }
    PIterContainter(int xstart, int ystart, int xend, int yend){
        EndIter = PointIterator(xstart, ystart, xend, yend);
        StartIter = EndIter;
    }
    PIterContainter(){
        EndIter = PointIterator();
        StartIter = EndIter;
    }
    PointIterator & begin(){
        return StartIter;
    }
    PointIterator & end(){
        return EndIter;
    }
};
/*
#define BoardIterate() PIterContainter()
#define SquareIterate(Cen,Range) PIterContainter(Cen,Range)
#define RectIterate(xstart,ystart,xend,yend) PIterContainter(xstart,ystart,xend,yend)
*/
//this type is there to allow better safety and conciseness for dealing with interations between RangeArrays and SquareIterates
struct BoardSquare{
    Point Cen;
    int Range;
    BoardSquare(Point InCen, int InRange){
        Cen = InCen;
        Range = InRange;
    }
    void operator =(BoardSquare & Other) = delete;//disallow equality to promote safety of the type
};
inline PIterContainter BoardIterate(){
    return PIterContainter();
}
inline PIterContainter SquareIterate(Point Cen,int Range){
    return PIterContainter(Cen,Range);
}
inline PIterContainter SquareIterate(BoardSquare S){
    return PIterContainter(S.Cen,S.Range);
}
inline PIterContainter RectIterate(int xstart,int ystart,int xend,int yend){
    return PIterContainter(xstart,ystart,xend,yend);
}
