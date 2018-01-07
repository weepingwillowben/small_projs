import tkinter as tk
import math

BOARD_SIZE = 600
SCORE_SIZE = 50
PLAYER_COLORS = ["red","yellow","blue","green"]
BACK_COLOR = "black"

def get_player_loc(p_num):
    x_loc = (p_num % 2) * 2 - 1
    y_loc = (p_num // 2) * 2 - 1
    return (x_loc,y_loc)
    
def convert_s(pos):
    return math.copysign(math.ceil(abs((pos + 0.00001)* BOARD_SIZE / 2)),pos)
   
def convert_x(pos):
    return convert_s(pos+1.0)

def convert_y(pos):
    return convert_s(-pos+1.0)
    
def convert_list(points):
    new_l = []
    for p in points:
        new_l.append(convert_x(p[0]))
        new_l.append(convert_y(p[1]))
        
    return new_l
    
class Console(object):
    def __init__(self,root):
        self.root = root
        self.root.config(bg=BACK_COLOR)
        self.score_canv = tk.Canvas(self.root,height=BOARD_SIZE,width=BOARD_SIZE+SCORE_SIZE*2,bg=BACK_COLOR,highlightbackground=BACK_COLOR)
        self.canv = tk.Canvas(self.score_canv,height=BOARD_SIZE,width=BOARD_SIZE,bg=BACK_COLOR,highlightbackground=BACK_COLOR)
        
        self.canv.place(x=SCORE_SIZE,y=0)
        self.score_canv.pack()
        
    def begin_draw(self):
        self.score_canv.delete("all")
        self.canv.delete("all")
        
    def draw_sheild(self,points,player_num):
        self.canv.create_polygon(convert_list(points),fill=PLAYER_COLORS[player_num],outline="")
        
    def draw_block(self,points):
        ypos = points[0][1]
        
        if abs(ypos) >= 1:
            return
        #creates a gradient of redness to whiteness of the blocks
        dis = int((1-abs(ypos))*255)
        block_color = "#" + "ff" + 2 * (hex(dis >> 4)[2:]+hex(dis & 0xf)[2:])
        
        self.canv.create_polygon(convert_list(points),fill=block_color,outline="")
        
    def draw_king(self,points,player_num):
        self.canv.create_polygon(convert_list(points),fill=PLAYER_COLORS[player_num],outline="")
        
    def draw_ball(self,cen,diameter):
        ball_psize = convert_s(diameter)
        xpix = convert_x(cen[0]) - ball_psize // 2
        ypix = convert_y(cen[1]) - ball_psize // 2
        
        self.canv.create_oval(xpix,ypix,xpix+ball_psize,ypix+ball_psize,fill="red",outline="yellow",dash=[2,2,2,2,2])
        
    def draw_score(self,score,player_num):
        loc = get_player_loc(player_num)
        xpix = int(convert_x(loc[0]*((BOARD_SIZE+SCORE_SIZE)/BOARD_SIZE))) + SCORE_SIZE
        ypix = int(convert_y(loc[1]*(BOARD_SIZE/(BOARD_SIZE+SCORE_SIZE))))
        self.score_canv.create_text((xpix,ypix),anchor="center",text=str(score),font=("Copperplate Gothic Bold",36),fill=PLAYER_COLORS[player_num])
        
    def draw_border(self):
        self.canv.create_rectangle(2,2,BOARD_SIZE+2,BOARD_SIZE+2,fill="",outline="BLUE",width=4)
    '''def draw(self,bloc_locs,sheild_locs,king_locs,ball_loc):
        self.canv.delete("ALL")
        for p_n,p in enumerate(sheild_locs):
            self.cons.draw_sheild(p,p_n)
        for p_n,p in enumerate(king_locs):
            self.cons.draw_king(p,p_n)
        for p in bloc_locs:
            self.cons.draw_block(p)
        self.cons.draw_ball(ball_loc)'''
        
class Interface(object):
    def __init__(self):
        self.root = tk.Tk()
        self.cons = Console(self.root)
        
        self.player_num = 0
        
        self.message_tick = 0
        
        self.prev_mp = BOARD_SIZE/2    #keeps track of previous mouse loction
        self.movement = 0   #keeps track of total mouse movement
        
        self.cons.root.bind("<Motion>",self.document_motion)
        
    def document_motion(self,event):
        self.movement += 2*(event.x - self.prev_mp) / BOARD_SIZE
        self.prev_mp = event.x
        
    def send_movement(self):
        #todo:implement http send info here
        game_messages[self.player_num].append((self.message_tick,self.movement))
        
        self.movement = 0
        self.message_tick += 1
        
    def get_message(self):
        self.cons.draw(graphics_infoserver_messages[self.player_num])


if __name__ == "__main__":
    cons = Interface()
    cons.cons.draw_sheild(0.2,-0.5,5,1)
    cons.cons.draw_ball(-0.2,0.5)
    cons.cons.draw_king(-0.2,-0.5,3)
    for n in range(-int(1/BLOCK_SIZE),int(1/BLOCK_SIZE)):
        cons.cons.draw_block(0.2,((BLOCK_SIZE)*n))
        
    cons.root.mainloop()






