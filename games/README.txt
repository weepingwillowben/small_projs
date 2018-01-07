This is an implementation of the Atari 2600 game WARLORDS(tm).

GameGUI.py is a Tkinter based GUI that tracks mouse movement in the 
x direction and draws the game on the screen.

GameSim.py is the main game code that updates the game state every frame.

PHYSICS:
Collision detection works by turing the path of the ball into a line 
segemnt, and checking which object edges intersect with it. It then 
bounces off of the nearest one. Both bouncing and detection add on 
the radius of the ball onto is position, however, collision detection view
the ball as a line, not a two dimentional object, so the ball's edges can 
sometimes appear to slide through objects, while the edges ususally bounce 
off of blocks. 

Note that currently, if a player moves their shield into where the ball 
was the previous frame, it will go right through the sheild! (rather
than it being trapped inside the shield). This makes lower frame-rates
basically unplayable.

FIX:
Ball bouncing is still not working properly at all, which kind of wreaks havoc 
on gameplay.