#pragma once
#include "PointOperators.h"
template<typename ArrayType>
class Array2d
{
public:
    //static version
    Array2d(ArrayType InitVal){
        Init(InitVal);
    }
    Array2d(void){
        //zeros out memory
        fill(begin(), end(), ArrayType());
    }
    ArrayType Arr[BoardSizeX][BoardSizeY];
    int Size(){
        return sizeof(ArrayType) * BoardSizeX * BoardSizeY;
    }
    void Init(ArrayType InitVal){
        fill(begin(), end(), InitVal);
    }
    ArrayType * begin(){
        return Arr[0];
    }
    ArrayType * end(){
        return &Arr[BoardSizeX - 1][BoardSizeY];
    }
    void operator = (const Array2d & InArr){
        //copy_n(InArr.begin(), BoardSizeX * BoardSizeY,begin());
        memcpy(Arr, InArr.Arr, BoardSizeX * BoardSizeY * sizeof(ArrayType));
    }
    ArrayType & operator [](Point & P){
        return Arr[P.X][P.Y];
    }
    ArrayType * operator[](int X){
        return Arr[X];
    }
    typedef ArrayType Ty;
    //macro hack allows easy making of operator overloads for Array2d
#define _ARRAY2D_OPEQUAL_GENERATOR_(Operator) \
    void operator Operator (const Array2d<Ty> & Op2){\
        for(int x = 0; x < BoardSizeX; x++)\
            for (int y = 0; y < BoardSizeY; y++)\
                Arr[x][y] Operator Op2.Arr[x][y];\
        }
    _ARRAY2D_OPEQUAL_GENERATOR_(+= )
    _ARRAY2D_OPEQUAL_GENERATOR_(*= )
    _ARRAY2D_OPEQUAL_GENERATOR_(/= )
    _ARRAY2D_OPEQUAL_GENERATOR_(-= )
#undef _ARRAY2D_OPEQUAL_GENERATOR_
#define _ARRAY2D_OP_GENERATOR_(Operator) \
    Array2d<Ty> operator Operator (const Array2d<Ty> & Op2){\
        Array2d<Ty> RetVal(0);\
        for(int x = 0; x < BoardSizeX; x++)\
            for (int y = 0; y < BoardSizeY; y++)\
                RetVal.Arr[x][y] = Arr[x][y] Operator Op2.Arr[x][y];\
        }
    _ARRAY2D_OP_GENERATOR_(+)
    _ARRAY2D_OP_GENERATOR_(*)
    _ARRAY2D_OP_GENERATOR_(/ )
    _ARRAY2D_OP_GENERATOR_(-)
#undef _ARRAY2D_OP_GENERATOR_
};

template<typename Ty1,typename Ty2,typename FnType>
void foreachArray2d(Array2d<Ty1> & First, Array2d<Ty2> & Second,FnType Fn){
    auto It1 = First.begin();
    auto It2 = Second.begin();
    auto End = First.end();
    for (; It1 < End - 4; It1 += 4, It2 += 4){
        Fn(*It1, *It2);
        Fn(*(It1 + 1), *(It2 + 1));
        Fn(*(It1 + 2), *(It2 + 2));
        Fn(*(It1 + 3), *(It2 + 3));
    }
    for (; It1 < End; It1++, It2++){
        Fn(*It1, *It2);
    }
}
//template<typename Ty1, typename Ty2, typename Ty3, typename Ty4, typename FnType>
//void foreachArray2d(Array2d<Ty1> & First, Array2d<Ty2> & Second, Array2d<Ty3> & Third, Array2d<Ty4> & Fourth, FnType Fn){
//	auto It1 = First.begin();
//	auto It2 = Second.begin();
//	auto It3 = Third.begin();
//	auto It4 = Fourth.begin();
//	auto End = First.end();
//	for (; It1 < End; It1++, It2++, It3++, It4++){
//		Fn(*It1, *It2, *It3, *It4);
//	}
//}
template<typename Container1, typename Container2, typename FnType>
void foreach_duo(Container1 & First, Container2 & Second, FnType Fn){
    auto It1 = First.begin();
    auto It2 = Second.begin();
    auto End1 = First.end();
    auto End2 = Second.end();
    for (; It1 != End1 && It2 != End2; It1++,It2++){
        Fn(*It1, *It2);
    }
}
template<typename AssociateType>
struct ValInfo{
    ValInfo(double StartVal, AssociateType InInfo){ Val = StartVal; Info = InInfo; };
    ValInfo(){ Val = -1e50; Info = AssociateType(); };
    ~ValInfo() = default;
    bool operator < (const ValInfo & Other){
        return this->Val < Other.Val;
    }
    bool operator > (const ValInfo & Other){
        return this->Val > Other.Val;
    }
    double Val;
    AssociateType Info;
};
template<typename ThingType,const size_t Size>
class ThingVals
    {
    public:
        array<double, Size> Vals;
        using iterator = typename map<double, ThingType>::reverse_iterator;
        ThingVals(){
            fill(Vals.begin(), Vals.end(),ThingType());
        }
        ThingType GetBest(){
            ValInfo<ThingType> MaxVal;

            for (int g = 0; g < Size; g++){
                if (MaxVal.Val < Vals[g])
                    MaxVal = ValInfo<ThingType>(Vals[g], static_cast<ThingType>(g));
            }
            return MaxVal.Info;
        }
        void InitSortVals(){
            SortedVals.clear();
            for (int g = 0; g < Size; g++)
                SortedVals[Vals[g]] = static_cast<ThingType>(g);
        }
        iterator begin(){
            InitSortVals();
            return SortedVals.rbegin();
        }
        iterator end(){
            return SortedVals.rend();
        }
        double & operator[](ThingType Attc){
            return Vals[static_cast<int>(Attc)];
        }
    protected:
        bool IsSorted = false;
        map<double, ThingType> SortedVals;
    };
template<typename ThingType>
class ThingList{
public:
    boost::container::flat_set<ThingType> Data;
    ThingList(){};
    ThingList(initializer_list<ThingType> StartList){
        for (ThingType ThisThing : StartList)
            add(ThisThing);
    }
    bool has(ThingType TestThing){
        return Data.count(TestThing);
    }
    void add(ThingType TestThing){
        Data.insert(TestThing);
    }
    void remove(ThingType TestThing){
        Data.erase(TestThing);
    }
    int size(){
        return Data.size();
    }
    void clear(){
        Data.clear();
    }
    bool operator == (ThingList & other){
        if (this->size() != other.size())
            return false;

        auto OIt = other.begin();
        for (auto & thing : *this){
            if (thing != *OIt)
                return false;
            ++OIt;
        }
        return true;
    }
    void save(ofstream & File){
        int size = Data.size();
        File.write((char *)(&size), sizeof(size));
        for (ThingType Val:Data)
            File.write((char *)(&Val), sizeof(Val));
    }
    void load(ifstream & File){
        int Size = 0;
        File.read((char *)(&Size), sizeof(Size));
        for (int n : range(Size)){
            ThingType Val = ThingType(0);
            File.read((char *)(&Val), sizeof(Val));
            Data.insert(Val);
        }
    }
    auto begin()->decltype(Data.begin()){
        return Data.begin();
    }
    auto end()->decltype(Data.begin()){
        return Data.end();
    }
};
template<typename Ty1,typename Ty2,typename Ty3>
struct triple{
    Ty1 first;
    Ty2 second;
    Ty3 third;
    triple(Ty1 infirst, Ty2 insecond, Ty3 inthird){
        first = infirst;
        second = insecond;
        third = inthird;
    }
};
template<typename InfoType>
struct PointInfo{
    Point P;
    InfoType * Data;
    PointInfo(){
        P = { 0, 0 };
        Info = NULL;
    };
    PointInfo(Point InP, InfoType * InInfo){
        P = InP;
        Data = InInfo;
    }
    InfoType & Info(){
        return *Data;
    }
    void SetInfo(InfoType InInfo){
        Info() = InInfo;
    }
};
template<typename ArrayType>
class RangeArray;

template<typename ArrayType>
class RA_Iterator{
public:
    PointIterator Spot;
    using ArrIterator = typename vector<ArrayType>::iterator;
    ArrIterator ArrIt;
    RA_Iterator(RangeArray<ArrayType> * InArr, bool Begin){
        ArrIt = InArr->Arr.begin();

        int sx = InArr->Corner.X;
        int sy = InArr->Corner.Y;
        int xdis = InArr->XSize;
        int ydis = InArr->YSize;
        Spot = PointIterator(sx, sy, sx + xdis - 1, sy + ydis - 1); //inclusive range here, so -1 is right
        if (!Begin)
            Spot = PointIterator(10, 10, 9, 9);
    }
    bool operator != (RA_Iterator & Other){
        return Spot.NotEnd();
    }
    void operator ++ (){
        ++Spot;
        ++ArrIt;
    }
    PointInfo<ArrayType> operator *(){
        return PointInfo<ArrayType>(*Spot, &(*ArrIt));
    }
};
template<typename ArrayType>
class RangeArray
{
public:
    using iterator = RA_Iterator<ArrayType>;
    vector<ArrayType> Arr;
    Point Corner;
    int YSize, XSize;
    RangeArray(Point InCenP,int InRange){
        Corner.X = max(InCenP.X - InRange, 0);
        Corner.Y = max(InCenP.Y - InRange, 0);

        XSize = min(InCenP.X + InRange + 1, BoardSizeX) - Corner.X;
        YSize = min(InCenP.Y + InRange + 1, BoardSizeY) - Corner.Y;

        Arr.resize(XSize * YSize);
        Init(ArrayType());//zeros out data
    }
    RangeArray(BoardSquare Sq) :RangeArray(Sq.Cen, Sq.Range){}
    RangeArray(){
        Corner = CreatePoint(0, 0);
        YSize = 0;
        XSize = 0;
    }
    RangeArray(const RangeArray & other){
        (*this) = other;
    }
    RangeArray(RangeArray && other){
        (*this) = other;
    }

    void operator =(const RangeArray & val){
        _copy_scalars(val);
        Arr = val.Arr;
    }
    void operator =(RangeArray && val){
        _copy_scalars(val);
        Arr = move(val.Arr);
    }
    void Init(ArrayType InitVal){
        for (auto & Val : Arr)
            Val = InitVal;
    }
    int Size(){
        return Arr.size();
    }
    int PointToInt(Point P){
        return (P.X - Corner.X) * YSize + (P.Y - Corner.Y);
    }
    iterator begin(){
        return iterator(this,true);
    }
    iterator end(){
        return iterator(this,false);
    }
    bool IsInScope(Point P){
        int Xadj = P.X - Corner.X;
        int Yadj = P.Y - Corner.Y;
        return Xadj < XSize && Yadj < YSize && Xadj >= 0 && Yadj >= 0;
    }
    ArrayType & operator [](Point & P){
        return Arr[PointToInt(P)];
    }
protected:
    void _copy_scalars(const RangeArray & other){
        Corner = other.Corner;
        YSize = other.YSize;
        XSize = other.XSize;
    }
};
template<typename ArrayType>
class PartialRangeArray;

template<typename ArrayType>
class PointPartialIterator:
    public RA_Iterator<ArrayType>{
public:
    vector<bool>::iterator BIter;
    PointPartialIterator(PartialRangeArray<ArrayType> * InArr, bool Begin) :RA_Iterator<ArrayType>(InArr, Begin){
        BIter = InArr->PointExists.begin();
        ContinueToNextReal();
    }
    void ContinueToNextReal(){
        if (this->Spot.NotEnd() && !(*BIter))
            ++(*this);//coninues on counting if it is not passed the end of itself and it is not at an existing spot
    }
    void operator ++ (){
        RA_Iterator<ArrayType>::operator++();
        ++BIter;
        ContinueToNextReal();
    }
};
template<typename ArrayType>
class PartialIterator{
public:
    vector<bool>::iterator BIter;
    vector<bool>::iterator BEndIt;
    typedef typename vector<ArrayType>::iterator ArrayIterator;
    ArrayIterator ArrIt;
    PartialIterator(PartialRangeArray<ArrayType> * InArr,bool Begin){
        BIter = InArr->PointExists.begin();
        BEndIt = InArr->PointExists.end();
        ArrIt = Begin ? InArr->Arr.begin() : InArr->Arr.end();
        ContinueToNextReal();
    }
    bool operator != (PartialIterator & Other){
        return BIter != BEndIt;
    }
    void ContinueToNextReal(){
        if (BIter != BEndIt && !(*BIter))
            ++(*this);//coninues on counting if it is not passed the end of itself and it is not at an existing spot
    }
    void operator ++ (){
        ++BIter;
        ++ArrIt;
        ContinueToNextReal();
    }
    ArrayType & operator*(){
        return *ArrIt;
    }
};
template<typename ArrayType>
class PartialRangeArray:
    public RangeArray<ArrayType>
{
public:
    using iterator = PointPartialIterator<ArrayType>;
    vector<bool> PointExists;
    using RArray = RangeArray<ArrayType>;
    PartialRangeArray(Point InCenP, int InRange) :RArray(InCenP,InRange){
        int Size = this->XSize * this->YSize;
        PointExists.resize(Size);
        PointExists.assign(Size,0);//assigns every spot to 0, or false
    }
    PartialRangeArray(BoardSquare Sq) :PartialRangeArray(Sq.Cen, Sq.Range){}
    PartialRangeArray() : RArray(){}
    PartialRangeArray(const PartialRangeArray & other){
        (*this) = other;
    }
    PartialRangeArray(PartialRangeArray && other){
        (*this) = other;
    }
    void operator =(const PartialRangeArray & val){
        RArray::operator =(val);
        PointExists = val.PointExists;
    }
    void operator =(PartialRangeArray && val){
        RArray::operator =(val);
        PointExists = move(val.PointExists);
    }
    void SetExist(Point P,bool Val){
        PointExists[this->PointToInt(P)] = Val;
    }
    int ActiveSize(){
        int Count = 0;
        for (bool Boolean : PointExists)
            Count += Boolean;
        return Count;
    }
    bool IsInScope(Point P){
        Point C = this->Corner;
        int Xadj = P.X - C.X;
        int Yadj = P.Y - C.Y;
        if (Xadj < this->XSize && Yadj < this->YSize && Xadj >= 0 && Yadj >= 0)
            return this->PointExists[this->PointToInt(P)];
        else
            return false;
    }
    iterator begin(){
        return iterator(this,true);
    }
    iterator end(){
        return iterator(this, false);
    }

};
/*
you can use this in the following way:

var global;//var is a type
void DoSomething(){
    auto Key = Protect(global);
    //change global
}//when Key destructs, it returns global to its previous value

it also allows for early restoring with Release, late protecting with
Protect, and switching keys that protect the global with
new = std::move(old)
*/
template<typename GlobType>
class ProtectedGlobal{
public:
    GlobType SavedGlobal;
    GlobType * Global;
    ProtectedGlobal(){
        Global = NULL;
    }
    ProtectedGlobal(GlobType & InGlob){
        Protect(InGlob);
    }
    ProtectedGlobal(ProtectedGlobal & Other) = delete;
    ProtectedGlobal(ProtectedGlobal && Other){
        *this = std::move(Other);
    }
    void operator = (ProtectedGlobal &) = delete;
    void operator = (ProtectedGlobal && Other){
        Global = Other.Global;
        SavedGlobal = Other.SavedGlobal;
        Other.Global = NULL;
    }
    ~ProtectedGlobal(){
        Release();
    }
    void Release(){
        if (Global != NULL){
            *Global = SavedGlobal;
            Global = NULL;
        }
    }
    void Protect(GlobType & InGlob){
        Release();
        SavedGlobal = InGlob;
        Global = &InGlob;
    }
};
template<typename GlobType>
inline ProtectedGlobal<GlobType> Protect(GlobType & InGlob){
    return ProtectedGlobal<GlobType>(InGlob);
}
