#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include<math.h>
#include<stdio.h>
#include<string.h>
#include"headers.h"
#include<string.h>
#include <SDL.h>

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	int t1, t2, quit, frames, rc, stop, size;
	double delta, worldTime, fpsTimer, fps;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *inside, *crate, *goal, *outside, *player, *readycrate, *wall;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	int posx, posy;		//describes player's position
	int previous_field = INSIDE; //neccessary only if a player walks on a "goal" field
	const char* board_file = "default_level.txt";		//name of the game file
	if (boardSize(board_file) == EOF) {	//check if the game level file was opened correctly
		printf("The .txt file is empty or error occured when opening the file. %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	else size = boardSize(board_file);

	int **board;		//array which stores all the information about the fields
	boardInitialization(board, size);

	readingBoardFile(board_file, board);
	posx = playerPositionX(board, size);		//current player position
	posy = playerPositionY(board, size);

	int current_position, moves = 0, new_game = 0;
	double finish_time = 0;


	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	printf("printf output goes here\n");
	printf("player x: %d\n", posx);
	printf("player y: %d\n", posy);

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	//  fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Sokoban, Piotr Danielczyk 171953");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	SDL_SetColorKey(charset, true, 0x000000);

	//loading graphics for fields
	inside = SDL_LoadBMP("./inside.bmp");
	crate = SDL_LoadBMP("./crate.bmp");
	goal = SDL_LoadBMP("./goal.bmp");
	outside = SDL_LoadBMP("./outside.bmp");
	player = SDL_LoadBMP("./player.bmp");
	readycrate = SDL_LoadBMP("./readycrate.bmp");
	wall = SDL_LoadBMP("./wall.bmp");

	//checking if the graphic files were loaded correctly
	if(inside == NULL || crate == NULL || goal == NULL || outside == NULL 
		|| player == NULL || readycrate == NULL || wall == NULL) {
		printf("Error when loading .bmp graphics %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	stop = 0;
	worldTime = 0;

	while (!quit) {
		if (stop == 0) {		//stop == 0 means that the current game hasn't been won yet.

			if (new_game == 1) {		//after clicking n the game file is read again
				worldTime = 0;			//and all the counters are resetted
				readingBoardFile(board_file, board);
				posx = playerPositionX(board, size);
				posy = playerPositionY(board, size);
				new_game = 0;
				moves = 0;
				t1 = SDL_GetTicks();
			}
			//prevents from removing the goal graphic after the player steps on it
			if (board[posy][posx] == PLAYER_ON_GOAL)
				current_position = GOAL;
			else current_position = INSIDE;

			t2 = SDL_GetTicks();

			// here t2-t1 is the time in milliseconds since
			// the last screen was drawn
			// delta is the same time in seconds
			delta = (t2 - t1) * 0.001;
			t1 = t2;
			worldTime += delta;

			SDL_FillRect(screen, NULL, czarny);

			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					switch (board[i][j]) {	//draws an appropriate graphic depending on what field it is (info taken from board array)
					case 0:
						DrawSurface(screen, outside, j*IMAGE_SIZE + GAME_X_POSITION, i*IMAGE_SIZE + GAME_Y_POSITION);
						break;
					case 1:
						DrawSurface(screen, wall, j*IMAGE_SIZE + GAME_X_POSITION, i*IMAGE_SIZE + GAME_Y_POSITION);
						break;
					case 2:
						DrawSurface(screen, inside, j*IMAGE_SIZE + GAME_X_POSITION, i*IMAGE_SIZE + GAME_Y_POSITION);
						break;
					case 3:
						DrawSurface(screen, crate, j*IMAGE_SIZE + GAME_X_POSITION, i*IMAGE_SIZE + GAME_Y_POSITION);
						break;
					case 4:
						DrawSurface(screen, goal, j*IMAGE_SIZE + GAME_X_POSITION, i*IMAGE_SIZE + GAME_Y_POSITION);
						break;
					case 5:
						DrawSurface(screen, player, j*IMAGE_SIZE + GAME_X_POSITION, i*IMAGE_SIZE + GAME_Y_POSITION);
						break;
					case 6:
						DrawSurface(screen, readycrate, j*IMAGE_SIZE + GAME_X_POSITION, i*IMAGE_SIZE + GAME_Y_POSITION);
						break;
					case 7:			//PLAYER_ON_GOAL's graphic is the same as PLAYER's
						DrawSurface(screen, player, j*IMAGE_SIZE + GAME_X_POSITION, i*IMAGE_SIZE + GAME_Y_POSITION);
						break;
					}
				}
			}

			fpsTimer += delta;
			if (fpsTimer > 0.5) {
				fps = frames * 2;
				frames = 0;
				fpsTimer -= 0.5;
			};

			// info text

			DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
			sprintf(text, "Elapsed time = %.1lf s  %.0lf frames / s, %d moves", worldTime, fps, moves);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
			sprintf(text, "Esc - quit, n - new game, \030 \031 \032 \033 - moving");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

				if ( checkIfWon(board, size) == YES) {	//if the game has been won:
				finish_time = worldTime;		//saves current time as finishing time
				stop = 1;
				DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, zielony);
				sprintf(text, "CONGRATULATIONS! You won! Your time was %.1lf s, you made %d moves", finish_time, moves);
				DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
				sprintf(text, "Press n to start a new game, press ESC to quit", finish_time);
				DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
				}

			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			//		SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

			// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
			while (SDL_PollEvent(&event)) {

				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_UP) {
						if (nextFieldFree(board, posx, posy - 1, size) == YES) { //checks if the destination field is free
							movementFreeField(board, posx, posy - 1);		//updates info in the board array
							board[posy][posx] = current_position;
							--posy; ++moves;		//moving the player and increasing the moves counter
						}
						else if ((posy - 1 > 0) && (board[posy - 1][posx] == CRATE || board[posy - 1][posx] == READYCRATE))
							if (nextFieldFree(board, posx, posy - 2, size) == YES) {
								movementWithCrate(board, posx, posy - 1, posx, posy - 2);
								board[posy][posx] = current_position;
								--posy; ++moves;
							}
						break;
					}
					else if (event.key.keysym.sym == SDLK_DOWN) {
						if (nextFieldFree(board, posx, posy + 1, size) == YES) {
							movementFreeField(board, posx, posy + 1);
							board[posy][posx] = current_position;
							++posy; ++moves;
						}
						else if ((posy + 1 < size) && (board[posy + 1][posx] == CRATE || board[posy + 1][posx] == READYCRATE))
							if (nextFieldFree(board, posx, posy + 2, size) == YES) {
								movementWithCrate(board, posx, posy + 1, posx, posy + 2);
								board[posy][posx] = current_position;
								++posy; ++moves;
							}
						break;
					}

					else if (event.key.keysym.sym == SDLK_LEFT) {
						if (nextFieldFree(board, posx - 1, posy, size) == YES) {
							movementFreeField(board, posx - 1, posy);
							board[posy][posx] = current_position;
							--posx; ++moves;
						}
						else if (board[posy][posx - 1] == CRATE || board[posy][posx - 1] == READYCRATE)
							if (nextFieldFree(board, posx - 2, posy, size) == YES) {
								movementWithCrate(board, posx - 1, posy, posx - 2, posy);
								board[posy][posx] = current_position;
								--posx; ++moves;
							}
						break;
					}
					else if (event.key.keysym.sym == SDLK_RIGHT) {
						if (nextFieldFree(board, posx + 1, posy, size) == YES) {
							movementFreeField(board, posx + 1, posy);
							board[posy][posx] = current_position;
							++posx; ++moves;
						}
						else if (board[posy][posx + 1] == CRATE || board[posy][posx + 1] == READYCRATE)
							if (nextFieldFree(board, posx + 2, posy, size) == YES) {
								movementWithCrate(board, posx + 1, posy, posx + 2, posy);
								board[posy][posx] = current_position;
								++posx; ++moves;
							}
						break;
					}
					else if (event.key.keysym.sym == SDLK_n) {
						new_game = 1;
						break;
					}

				};
				if (event.type == SDL_QUIT) quit = 1;
			};
			frames++;
		}
				//if the game has been won: waits until n or ESC is pressed
			while (stop == 1 && SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_n) {
						stop = 0;
						new_game = 1;
						break;
					}
				}
				if (event.type == SDL_QUIT) quit = 1;
			}
	}

	//freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};