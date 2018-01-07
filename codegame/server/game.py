from server import board
import enum
import collections
import json

Feature = collections.namedtuple("Feature",['filename','name','ord'])

background = Feature(name="background",filename='/static/pictures/Background.bmp',ord=0)
troop = Feature(name="cata",filename='/static/pictures/Catapult.png',ord=1)


class BoardSquare:
    def __init__(self):
        self.feature_set = [background,troop]
    def to_json(self):
        self.feature_set.sort(key=lambda x:x.ord)
        return [feat.filename  for feat in self.feature_set]

class Game:
    def __init__(self,bdim):
        self.bdim = bdim
        self.players = []
        self.board = board.Array2d(bdim)
        self.set_init_units()

    def set_init_units(self):
        for p in self.bdim.board():
            self.board[p] = BoardSquare()

    def to_json(self):
        bdata = [[self.board.arr[y][x].to_json()
                    for x in range(self.bdim.x)]
                        for y in range(self.bdim.y)]
        all_data = {
            "boardsize_x":self.bdim.x,
            "boardsize_y":self.bdim.y,
            "board":bdata,
        }
        return json.dumps(all_data)


cur_game = Game(board.BoardDim(10,10))
def new_game(x,y):
    global cur_game
    cur_game = Game(board.BoardDim(x,y))
