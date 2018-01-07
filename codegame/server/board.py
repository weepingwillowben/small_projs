class Point:
    def __init__(self,x,y):
        self.x = x
        self.y = y
    def __add__(self,p):
        return Point(self.x+p.x,self.y+p.y)
    def __sub__(self,p):
        return Point(self.x-p.x,self.y-p.y)
    def __mul__(self,x):
        return Point(self.x*x,self.y*x)

class BoardDim(Point):
    def board(self):
        for x in range(self.x):
            for y in range(self.y):
                yield(Point(x,y))

class Array2d:
    def __init__(self,bdim):
        self.arr = [[None]*bdim.x for y in range(bdim.y)]
    def __getitem__(self,key):
        return self.arr[key.y][key.x]
    def __setitem__(self,key,value):
        self.arr[key.y][key.x] = value
    def __delitem__(self,key):
        self.arr[key.y][key.x] = None
