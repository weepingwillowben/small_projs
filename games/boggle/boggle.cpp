#include "C:/Users/Benjamin/Documents/Visual Studio 2013/RangeIterator.h"
#include <cstdlib>
#include <unordered_set>
#include <iostream>
#include <array>
#include <algorithm>
#include <fstream>
#include <unordered_map>
using namespace std;


constexpr int BOARD_SIZE = 4;
constexpr int SIDES = 6;
constexpr int NUM_DICE = BOARD_SIZE*BOARD_SIZE;
constexpr char dice[NUM_DICE][SIDES] = {
    {'A','A','C','I','O','T'},	
    {'A','B','I','L','T','Y'},	
    {'A','B','J','M','O','Q'},
    {'A','C','D','E','M','P'},	
    {'A','C','E','L','R','S'},	
    {'A','D','E','N','V','Z'},	
    {'A','H','M','O','R','S'},	
    {'B','I','F','O','R','X'},	
    {'D','E','N','O','S','W'},	
    {'D','K','N','O','T','U'},	
    {'E','E','F','H','I','Y'},	
    {'E','G','K','L','U','Y'},	
    {'E','G','I','N','T','V'},	
    {'E','H','I','N','P','S'},	
    {'E','L','P','S','T','U'},	
    {'G','I','L','R','U','W'}
};
typedef array<array<char,BOARD_SIZE>,BOARD_SIZE> board_ty;

board_ty generate_random_board(){
    unordered_set<int> taken;
	board_ty board;
    for (int y : range(BOARD_SIZE)){
        for (int x : range(BOARD_SIZE)){
			int n;
			do{
				 n = rand() % NUM_DICE;
			}
            while (taken.count(n));
                
            taken.emplace(n);
            int side = rand() % SIDES;
            board[y][x] = dice[n][side];
		}
	}
	return board;
}

void draw_board(board_ty & board){
    for (int y : range(BOARD_SIZE)){
        string Str = "";
        for (int x : range(BOARD_SIZE)){
            char letter = board[y][x];
			if (letter == 'Q')
				Str += "QU";
			else
				Str += string(1,letter) + " ";
		}
        cout << Str << "\n";
	}
}

bool has_word_help(char * rem_word,board_ty & board,int x,int y){
    if (*rem_word == 0)
        return true;
    if (board[y][x] == 0)
        return false;
    char letter = board[y][x];
    if (rem_word[0] == letter and (letter != 'Q' or rem_word[1] == 'U')){
		board[y][x] = 0;
        for (int new_y : range(max(0,y-1),min(BOARD_SIZE,y+2)))
            for (int new_x : range(max(0,x-1),min(BOARD_SIZE,x+2)))
                if (has_word_help(rem_word + 1 + (letter == 'Q'),board,new_x,new_y)){
					board[y][x] = letter;
                    return true;
				}
		board[y][x] = letter;
	}
    return false;
}
bool has_word(board_ty & board,string & word){
    for (int y : range(BOARD_SIZE))
        for (int x : range(BOARD_SIZE))
            if (has_word_help(word.data(),board,x,y))
                return true;
                
    return false;
}
    
int get_score(string & word){
    int l = word.size();
    if (l == 3)
        return 1;
    else if (l < 7)
        return l - 3;
    else if (l == 7)
        return 5;
    else
        return 11;
}
int main(){
	ifstream file("dictionary.txt");
	if(!file)
		return 1;
	unordered_set<string> dictionary;
	string buff;
	while(getline(file,buff))
		dictionary.emplace(buff);
	
	file.close();
		
	while (true){
		board_ty board = generate_random_board();
		draw_board(board);
		int total_score = 0;
		while (true){
			string word;
			cin >> word;
			std::transform(word.begin(), word.end(),word.begin(), ::toupper);
			if (word == "0")
				break;
			else if (word == "1"){
				int tot_points = 0;
				for (string word : dictionary){
					if (word.size() >= 3 and has_word(board,word)){
						cout << word << endl;
						tot_points += get_score(word);
					}	
				}
				cout << "total possible points = " << tot_points << endl;
			}
			else if (word.size() <= 2)
				cout << "invalid word, must be of length 3 or longer\n";
			else if (not dictionary.count(word))
				cout << "Not a real word\n";
			else if (not has_word(board,word))
				cout << "Not present on dice\n";
			else{
				cout << "word scored " << get_score(word) << " points\n";
				total_score += get_score(word);
			}
		}
		cout << "scored " << total_score << " points in game\n\n";
	}
	return 0;
}

