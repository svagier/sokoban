#pragma once
#include <SDL.h>
#define SCREEN_WIDTH	720
#define SCREEN_HEIGHT	480
#define IMAGE_SIZE		36		//length of 1 square field graphic in pixels
#define GAME_X_POSITION 40		//enables to make adjustments to game graphics position on the screen
#define GAME_Y_POSITION 80

#define NO 0
#define YES	1

int boardSizeX(const char* file_name);
int boardSizeY(const char* file_name);
int boardSize(const char* file_name);
int boardInitialization(int ** &new_board, int size);
void readingBoardFile(const char* file_name, int **board);
int playerPositionX(int **board, int size);
int playerPositionY(int **board, int size);
int nextFieldFree(int **board, int posx, int posy, const int size);
void movementFreeField(int **board, int next_x, int next_y);
void movementWithCrate(int **board, int posx, int posy, int next_x, int next_y);
int checkIfWon(int **board, int size);

enum imageType {		//gives different numbers to different types of graphics
	OUTSIDE,
	WALL,
	INSIDE,
	CRATE,
	GOAL,
	PLAYER,
	READYCRATE,
	PLAYER_ON_GOAL
};

//default functions from the template
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
	SDL_Surface *charset);
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y);
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color);
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color);

void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor);