import random
import copy

BOARD_SIZE = 4
NUM_DICE = BOARD_SIZE*BOARD_SIZE
dice = [
    ['A','A','C','I','O','T'],	
    ['A','B','I','L','T','Y'],	
    ['A','B','J','M','O','QU'],
    ['A','C','D','E','M','P'],	
    ['A','C','E','L','R','S'],	
    ['A','D','E','N','V','Z'],	
    ['A','H','M','O','R','S'],	
    ['B','I','F','O','R','X'],	
    ['D','E','N','O','S','W'],	
    ['D','K','N','O','T','U'],	
    ['E','E','F','H','I','Y'],	
    ['E','G','K','L','U','Y'],	
    ['E','G','I','N','T','V'],	
    ['E','H','I','N','P','S'],	
    ['E','L','P','S','T','U'],	
    ['G','I','L','R','U','W']
]


def generate_random_board():
    taken = set()
    board = []
    for y in range(BOARD_SIZE):
        slice = []
        for x in range(BOARD_SIZE):
            n = random.randint(0,NUM_DICE-1)
            while n in taken:
                n = random.randint(0,NUM_DICE-1)
                
            taken.add(n)
            side = random.randint(0,6-1)
            slice.append(dice[n][side])
        board.append(slice)
        
    return board
    
def draw_board(board):
    for y in range(BOARD_SIZE):
        Str = ""
        for x in range(BOARD_SIZE):
            letter = board[y][x]
            Str += letter + (2-len(letter)) * " "
        print(Str)

def has_word_help(rem_word,board,x,y):
    if not rem_word:
        return True
    if board[y][x] == None:
        return False
    letter = board[y][x]
    if rem_word[0] == letter and (letter != 'QU' or rem_word[1] == 'U'):
        board[y][x] = None
        for new_y in range(max(0,y-1),min(BOARD_SIZE,y+2)):
            for new_x in range(max(0,x-1),min(BOARD_SIZE,x+2)):
                if has_word_help(rem_word[len(letter):],board,new_x,new_y):
                    board[y][x] = letter
                    return True
        board[y][x] = letter
        
    return False
    
def has_word(board,word):
    for y in range(BOARD_SIZE):
        for x in range(BOARD_SIZE):
            if has_word_help(word.upper(),board,x,y):
                return True
                
    return False
    
def get_score(word):
    l = len(word)
    if l == 3:
        return 1
    elif l < 7:
        return l - 3
    elif l == 7:
        return 5
    else:
        return 11
    
file = open("dictionary.txt")
dictionary = {line.strip() for line in file.readlines()}
file.close()
random.seed(0)
while True:
    board = generate_random_board()
    draw_board(board)
    total_score = 0
    while True:
        word = input()
        if word == "0":
            break
        elif word == "1":
            tot_points = 0
            for word in dictionary:
                if len(word) >= 3 and has_word(board,word):
                    print(word)
                    tot_points += get_score(word)
            print("total possible points = ",tot_points)
        elif len(word) <= 2:
            print("invalid word, must be of length 3 or longer")
        elif word.upper() not in dictionary:
            print("Not a real word")
        elif not has_word(board,word):
            print("Not present on dice")
        else:
            print("word scored ",get_score(word)," points")
            total_score += get_score(word)

    print("scored ",total_score," points in game\n")



