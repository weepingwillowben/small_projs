import GameGUI as GUI
import random
import math
import tkinter
import copy

import math

BLOCK_SIZE = 0.08
KING_SIZE = 0.1
SHEILD_WIDTH = 0.08
SHEILD_HEIGHT = 0.06
SHEILD_EDGE_WIDTH = 0.05
BALL_SIZE = 0.05

FORT_THICKNESS = 4
SHIELD_LOC = 0.2
SHIELD_MIDDLE = 0.2
FORT_HEIGHT = 6
NUM_PLAYERS = 4
BALL_VELOCITY = 1.0
FPS = 5
game_messages = [[]]
server_messages = [[]]

def get_player_loc(p_num):
    x_loc = (p_num % 2) * 2 - 1
    y_loc = (p_num // 2) * 2 - 1
    return (x_loc,y_loc)
    
def add_mirror(points):
    new_ps = points
    for x,y in points[-2:1:-1]:#go through all of the list except the first and the last, backwards
        new_ps.append((-x,y))
    return new_ps
    
def rotate(point,rotation):
    cos_t = math.cos(rotation)
    sin_t = math.sin(rotation)
    x,y = point
    return (x * cos_t - y * sin_t,x * sin_t + y * cos_t)
    
def rotate_all(points,rotation):
    return [rotate(p,rotation) for p in points]
    
def get_sheild_points(rotation):
    swt = SHEILD_WIDTH
    sw = swt / 2
    sew = SHEILD_EDGE_WIDTH
    sht = SHEILD_HEIGHT
    sh = sht / 2
    sep = sew + sw
    points = [
        (-sw,sh),
        (sw,sh),
        (sep,-sh),
        (sep,-sht-sh),
        (sw,-sh),
        (-0,-sh)
    ]
    points = add_mirror(points)
    points = rotate_all(points,rotation)
    return points
    
def get_distance(p1,p2):
    x1,y1 = p1
    x2,y2 = p2
    return math.sqrt((x1-x2) ** 2 + (y1-y2) ** 2)
    
def convert(edges):
    return [e.p1 for e in edges]
    
def div(n,d):
    if d == 0:
        if n == 0:
            raise ZeroDivisionError
        else:
            return math.copysign(float("inf"),n)
    else:
        return n / d
    
def sign(num):
    return math.copysign(1.0,num)
    
def quadrant(p):
    x,y = p
    return sign(x),sign(y)
    
class Edge(object):
    def __init__(self,p1,p2,obj=None):
        x1,y1 = self.p1 = p1
        x2,y2 = self.p2 = p2
        
        diffx = (x1 - x2)
        diffy = (y1 - y2)
        diff_tot = get_distance((diffx,diffy),(0,0))
        self.vec = diffx / diff_tot, diffy / diff_tot
        
        self.x_max = max(x1,x2)
        self.x_min = min(x1,x2)
        self.y_max = max(y1,y2)
        self.y_min = min(y1,y2)
        self.m = div(diffy , diffx)
        self.b = -self.m * x1 + y1
        self.parent_shape = obj
        
    def does_intersect(self,ball_vec):
        x_inter, y_inter = self.find_intersect(ball_vec)
        return (max(self.x_min,ball_vec.x_min) <= x_inter <= min(self.x_max,ball_vec.x_max) and 
                max(self.y_min,ball_vec.y_min) <= y_inter <= min(self.y_max,ball_vec.y_max))
                
    def find_intersect(self,ball_vec):
        if abs(ball_vec.m) == abs(self.m):
            return (float("inf"),float("inf"))
        if abs(self.m) == float("inf"):
            x_inter = self.x_max
        else:
            x_inter = div((ball_vec.b - self.b), (self.m - ball_vec.m))
            
        y_inter = ball_vec.m * x_inter + ball_vec.b
        return (x_inter,y_inter)
    
    def get_angle(self):
        return math.atan2(self.m,1.0)
        
class Ball(object):
    def __init__(self,center):
        self.center = center
        velo = BALL_VELOCITY / math.sqrt(2)
        
        self.velocity = (velo,velo)
        self.move_time = 0
        
    def start_move(self):
        self.move_time = 1 / FPS
        
    def get_ball_vec(self):
        dt = self.move_time
        p1 = self.center
        cx,cy = self.center
        vx,vy = self.velocity
        rx,ry = self.get_radius_dis()
        x = cx + vx * dt + rx
        y = cy + vy * dt + ry
        p2 = (x,y)
        return Edge(p1,p2)
        
    def get_radius_dis(self):
        vt = BALL_VELOCITY
        vx,vy = self.velocity
        r = BALL_SIZE / 2
        return (vx / vt) * r,(vy / vt) * r
        
    def reflect(self,edge):
        this_vec = self.get_ball_vec()
        x,y = edge.find_intersect(this_vec)
        rx,ry = self.get_radius_dis()
        new_p = (x-rx,y-ry)
        self.move_time -= get_distance(new_p,self.center) / BALL_VELOCITY
        
        self.center = new_p
        tvx,tvy = this_vec.vec
        evx,evy = edge.vec
            
        e_ang = edge.get_angle()
        t_ang = this_vec.get_angle()
        #remember that it is the normal, so the coords are flipped
        if get_distance((-evy,evx),this_vec.vec) < get_distance((evy,-evx),this_vec.vec):
            e_norm = (-evy,evx)
        else:
            e_norm = (evy,-evx)
        
        n_ang = Edge((0,0),e_norm,None).get_angle()
        
        new_ang = e_ang - (n_ang - t_ang)
        
        self.velocity = math.cos(new_ang) * BALL_VELOCITY,math.sin(new_ang) * BALL_VELOCITY
        
    def liniar_move(self):
        x,y = self.center
        vx,vy = self.velocity
        dt = self.move_time
        self.center = x + vx * dt, y + vy * dt
        self.move_time = 0
        
    def has_movement(self):
        return self.move_time > 0.001

class Cube(object):
    def __init__(self,center,size):
        cenx,ceny = center
        hsize = size / 2
        p1 = (cenx+hsize,ceny+hsize)
        p2 = (cenx-hsize,ceny+hsize)
        p3 = (cenx-hsize,ceny-hsize)
        p4 = (cenx+hsize,ceny-hsize)
        self.edges = [
            Edge(p1,p2,self),
            Edge(p2,p3,self),
            Edge(p3,p4,self),
            Edge(p4,p1,self)
        ]
        
class King(Cube):
    def __init__(self,center,size,player):
        self.p_num = player
        Cube.__init__(self,center,size)
   
class Shield(object):
    def __init__(self,player):
        self.loc = 0
        self.p_num = player
        self.edges = []
        self.center = (0,0)
        self.move(0)#places the edges for the first time
        
    def move(self,movement):
        self.loc += movement
        
        loc = self.loc = min(1.0,max(-1.0,self.loc))
        
        px,py = get_player_loc(self.p_num)
        sx = px * SHIELD_LOC
        sy = py * SHIELD_LOC
        off = (1-SHIELD_LOC) * abs(loc)
        
        if (loc > 0) == (px > 0):
            sx += off * px
            rotation = math.pi * (1+py) / 2
        else:
            sy += off * py
            rotation = px * math.pi / 2
            
        if -SHIELD_MIDDLE < loc < SHIELD_MIDDLE:
            rotation = (math.pi/4 + (math.pi/2)*(py > 0)) * px
            
        self.center = (sx,sy)
        
        edges = get_sheild_points(rotation)
        edges = [(x+sx,y+sy) for x,y in edges]
        self.edges = []
        prev_p = edges[-1]
        for p in edges:
            self.edges.append(Edge(prev_p,p,self))
            prev_p = p
    
    def is_interior(self,point):
        count = 0
        for e in self.edges:
            if e.does_intersect(Edge(point,(point[0]+100,point[1]))):
                count += 1
        return count % 2 == 1
            
class Borders(object):
    def __init__(self):
        p1 = (-1,1)
        p2 = (-1,-1)
        p3 = (1,-1)
        p4 = (1,1)
        self.edges = [
            Edge(p1,p2,self),
            Edge(p2,p3,self),
            Edge(p3,p4,self),
            Edge(p4,p1,self)
        ]
        
    def get_edges(self):
        return self.edges
     
class Blocks(object):
    def __init__(self):
        self.boxes = []
        FORT_SIZE = FORT_HEIGHT + FORT_THICKNESS
        for p in range(NUM_PLAYERS):
            px,py = get_player_loc(p)
            for xn in range(FORT_SIZE):
                x = (1-xn*BLOCK_SIZE)*px
                for yn in range(FORT_SIZE):
                    y = (1-yn*BLOCK_SIZE)*py
                    if xn > FORT_HEIGHT or yn > FORT_HEIGHT:
                        self.boxes.append(Cube((x,y),BLOCK_SIZE))
                        
    def get_edges(self):
        edges = []
        for b in self.boxes:
            edges += b.edges
        return edges
        
    def hit(self,block):
        self.boxes.remove(block)
        
class Player(object):
    def __init__(self,p_num,reflect):
        self.reflect = reflect
        self.p_num = p_num
        self.score = 0
        px,py = get_player_loc(p_num)
        #ensures that king is in center of fort
        offset = (1-FORT_HEIGHT*BLOCK_SIZE / 2)
        self.king = King((px*offset,py*offset),KING_SIZE,p_num)
        self.shield = Shield(p_num)
        self.active = True
        
    def has_lost(self):
        self.active = False
        
    def get_edges(self,ball_cen):
        if self.active:
            edges = copy.copy(self.king.edges)
            #if the ball is inside a shield already, then it just passes through
            if not self.shield.is_interior(ball_cen):
                edges += self.shield.edges
            return edges
        else:
            return []
            
    def reset(self):
        self.active = True
        
class AIPlayer(Player):
    def move_sheild(self,game):
        x,y = game.ball.get_ball_vec().p2
        px,py = get_player_loc(self.p_num)
        cx,cy = self.shield.center
        
        #moves sheild away from ball if it is already through its defenses, so it doesn't try to keep it in
        if (px,py) == quadrant((x,y)) and abs(x) > abs(px)*SHIELD_LOC and abs(y) > abs(py)*SHIELD_LOC:
            direction = -1
        else:
            direction = 1
        
        if sign(px) == sign(x) and abs(x) > SHIELD_LOC:
            delta = x - cx
        elif sign(py) == sign(y) and abs(y) > SHIELD_LOC:
            delta = y - cy
            if px * py > 0:
                delta = -delta
        else:
            #center on zero
            delta = px * SHIELD_LOC - cx + py * SHIELD_LOC - cy
        
        move_const = 1/(1-SHIELD_LOC)
        delta_loc = move_const * delta * direction
        
        self.shield.move(delta_loc)

class HumanPlayer(Player):
    def move_sheild(self,move_dis):
        self.shield.move(move_dis)
        
class Game(object):
    def __init__(self):
        self.p_inter = GUI.Interface()
        self.ball = Ball((0,0))
        self.AI_players = [AIPlayer(n,self.reflect) for n in range(1,NUM_PLAYERS)]
        self.hum_players = [HumanPlayer(0,self.reflect)]
        self.players = self.hum_players + self.AI_players
        
        self.forts = Blocks()
        self.border = Borders()
        
        self.game_tick = 0
        self.update()
        
    def reset(self):
        for p in self.players:
            p.reset()
        self.forts = Blocks()
        
    def update(self):
        '''for m in game_messages:
            if len(m) <= self.game_tick:
                self.p_inter.root.after(1,self.update)
                return'''
        
        #for hp,m in zip(self.hum_players,game_messages):
        #    hp.move_sheild(m[self.game_tick][1])
        self.hum_players[0].move_sheild(self.p_inter.movement)
        self.p_inter.movement = 0
            
        for ai in self.AI_players:
            ai.move_sheild(self)
        
        self.ball.start_move()
        while self.ball.has_movement():
            self.reflect_nearest_edge()
        
        if self.is_end():
            self.end_game()
            
        self.game_tick += 1
        
        self.show_graphics()
        
        self.p_inter.root.after(1000//FPS,self.update)
            
    def end_game(self):
        for p in self.players:
            if p.active:
                p.score += 1
        self.reset()
        
    def is_end(self):
        active_players = 0
        for p in self.players:
            if p.active:
                active_players += 1
                
        return active_players <= 1
            
    def find_closest_edge(self,all_edges,ball_vec):
        min_e = None
        min_dis = 10000.0
        for e in all_edges:
            if e.does_intersect(ball_vec):
                p_i = e.find_intersect(ball_vec)
                dis = get_distance(p_i,self.ball.center)
                if dis < min_dis:
                    min_dis = dis
                    min_e = e
        return min_e
        
    def reflect_nearest_edge(self):
        ref_edge = self.find_closest_edge(self.get_edges(), self.ball.get_ball_vec())
        if ref_edge:
            obj = ref_edge.parent_shape
            obj_ty = type(obj)
            if obj_ty == Cube:
                self.forts.hit(obj)
            elif obj_ty == King:
                self.players[obj.p_num].has_lost()
                
            self.ball.reflect(ref_edge)
        else:
            self.ball.liniar_move()
        
    def get_edges(self):
        edges = []
        for p in self.players:
            edges += p.get_edges(self.ball.center)
        edges += self.forts.get_edges()
        edges += self.border.get_edges()
        return edges
        
    def reflect(self,edge):
        self.ball.reflect(edge)
        
    def show_graphics(self):
        cons = self.p_inter.cons
        cons.begin_draw()
        pinfo = []
        for p in self.players:
            cons.draw_score(p.score,p.p_num)
            if p.active:
                cons.draw_king(convert(p.king.edges),p.p_num)
                cons.draw_sheild(convert(p.shield.edges),p.p_num)
        
        binfo = []
        for b in self.forts.boxes:
            cons.draw_block(convert(b.edges))
            
        cons.draw_ball(self.ball.center,BALL_SIZE)
        
        cons.draw_border()
        
G = Game()
G.p_inter.root.mainloop()