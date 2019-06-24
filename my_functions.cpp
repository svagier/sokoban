#include<math.h>
#include<stdio.h>
#include<string.h>
#include"headers.h"
#include <fstream>
#include <string>
#include<iostream>
using namespace std;

//checks number of lines (board Y size)
int boardSizeY(const char* file_name) {
	string line;
	int y = 0;
	ifstream myfile(file_name);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
			y++;
		myfile.close();
	}
	else {
		printf("Error when opening game level .txt file %s\n", SDL_GetError());
		return EOF;
	}
	return y;
}

//checks maximum number of characters in a line (X size)
int boardSizeX(const char* file_name) {
	ifstream myfile;
	char c;

	myfile.open(file_name);
	if (!myfile.is_open()) return EOF;
	int x = 0;
	int z = 0;
	myfile.get(c);
	while (myfile) {
		z = 0;
		while (myfile && c != '\n') {
			z = z + 1;
			myfile.get(c);
			if (z > x) x = z;
		}

		myfile.get(c);
	}
	myfile.close();
	return x;
}

//checks the mazximum size (in case the board is not a square)
int boardSize(const char* file_name) {
	if (boardSizeX(file_name) == EOF || boardSizeY(file_name) == EOF) return EOF;
	else if (boardSizeX(file_name) == 0 || boardSizeY(file_name) == 0) return EOF;
	else if (boardSizeX(file_name) > boardSizeY(file_name))
		return boardSizeX(file_name);
	else return boardSizeY(file_name);
}

int boardInitialization(int ** &new_board, int size) {
	new_board = new int*[size];
	for (int i = 0; i < size; i++)
		new_board[i] = new int[size];

	return 0;
}

void readingBoardFile(const char* file_name, int **board) {

	ifstream myfile;
	myfile.open(file_name);
	char c;
	int x = 0;
	int y = 0;
	myfile.get(c);
	while (myfile) {
		while (myfile && c != '\n') {
			board[y][x] = ((int)c - 48);	//associates the board array cell with a proper fiekld type (image)
			x++;
			myfile.get(c);
		}
		x = 0;
		y++;
		myfile.get(c);
	}
	myfile.close();
}

//swaps the info in the board array corresponding to the graphics
void movementFreeField(int **board, int next_x, int next_y) {	
			if (board[next_y][next_x] == INSIDE)
				board[next_y][next_x] = PLAYER;

			else if (board[next_y][next_x] == GOAL)
				board[next_y][next_x] = PLAYER_ON_GOAL;
}

//swaps the info in the board array corresponding to the graphics
void movementWithCrate(int **board, int posx, int posy, int next_x, int next_y) {
			 if (board[posy][posx] == CRATE) {
				switch (board[next_y][next_x]) {
				case INSIDE:
					board[next_y][next_x] = CRATE;
					board[posy][posx] = PLAYER;
					break;
				case GOAL:
					board[next_y][next_x] = READYCRATE;
					board[posy][posx] = PLAYER;
					break;
				}
			}

			else if (board[posy][posx] == READYCRATE) {
				switch (board[next_y][next_x]) {
				case INSIDE:
					board[next_y][next_x] = CRATE;
					board[posy][posx] = PLAYER_ON_GOAL;
					break;
				case GOAL:
					board[next_y][next_x] = READYCRATE;
					board[posy][posx] = PLAYER_ON_GOAL;
					break;
				}
			}
}

//checks if the destination field is free to move there
int nextFieldFree(int **board, int posx, int posy, const int size) {
	if (posy < 0 || posx < 0 || posy >= size || posx >= size) return NO;
	if (board[posy][posx] != INSIDE && board[posy][posx] != GOAL)
		return NO;
	else return YES;
}


int playerPositionX(int **board, int size) {

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (board[i][j] == PLAYER)
				return j;
		}
	}
	return -1;
}

int playerPositionY(int **board, int size) {

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (board[i][j] == PLAYER)
				return i;
		}
	}
	return -1;
}

int checkIfWon(int **board, int size) {
	int goals = 0, ready_crates = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (board[i][j] == GOAL  || board[i][j] == PLAYER_ON_GOAL)
				goals++;
			if (board[i][j] == READYCRATE)
				ready_crates++;
		}	//if all the goal fields are covered with crates the game is won
	}
	if (goals == 0 && ready_crates > 0)
		return YES;
	else return NO;
}

