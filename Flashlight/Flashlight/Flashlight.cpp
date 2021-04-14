
#include <stdio.h>
#include <conio.h>
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <math.h>
#include <time.h>
#include <malloc.h>


// size of window
const int SCREEN_WIDTH = 760;
const int SCREEN_HEIGHT = 560;

//Starts up SDL and creates window
int init();

//FUNCTION THAT ARE RESPONSIBLE FOR ALL WITH MEDIA-------------
	//create texture
SDL_Texture* loadTexture(std::string path);

//draw image from file to screen
void loadFromFile(int x, int y, SDL_Texture* tex, SDL_Renderer* renderer, int w, int h);

// loading Texture
void loadText(std::string fontType, int sizeOfFont, std::string text, SDL_Color textColor, SDL_Texture* textTex, int x, int y, int w, int h);

//FUNCTION THAT ARE RESPONSIBLE FOR MOUSE EVENT AND PLAYIG FIELD------------------------------
	//mouse event handling
void handleMouseEvent(int** playingField, int playingFieldLines, int playingFieldColumns, SDL_Texture* extinct_flashlight, SDL_Texture* lit_flashlight, int sizeOfDstRect);

//mouse position relative to the button
int checkMouseLocation(int xButtonPos, int yButtonPos, int widhtButton, int heightButton);

//check whether all the flashlight are lit
int checkPlayingFieldArray(int** playingField, int  playingFieldLines, int playingFieldColumns);

//FREES MEDIA AND SHUTS DOWNSDL------------------
void close();
void freeTexture(SDL_Texture* texture);
void freeGamingField(int** gamingField, int numberColumns);

// GLOBAL VARIABLES---------------------
	//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font* gFont = NULL;

int init()
{
	//Initialization flag
	int success = 1;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = 0;
	}
	//Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		success = 0;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Flashlights", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = 0;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = 0;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = 0;
				}
				//Initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
			}
		}
	}
	return success;
}


void loadFromFile(int x, int y, SDL_Texture* tex, SDL_Renderer* renderer, int w, int h) {
	SDL_Rect renderQuad = { x, y,w, h };
	SDL_RenderCopy(gRenderer, tex, NULL, &renderQuad);
}

void loadText(std::string fontType, int sizeOfFont, std::string text, SDL_Color textColor, SDL_Texture* textTex, int x, int y, int w, int h) {
	//Open the font
	gFont = TTF_OpenFont(fontType.c_str(), sizeOfFont);

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text.c_str(), textColor);
	//Create texture from surface pixels
	textTex = SDL_CreateTextureFromSurface(gRenderer, textSurface);
	//Get rid of old surface
	SDL_FreeSurface(textSurface);

	loadFromFile(x, y, textTex, gRenderer, w, h);

	//Update screen
	SDL_RenderPresent(gRenderer);
}

//coordinates to compare
int x_oldMousePos = -1, y_oldMousePos = -1;

void handleMouseEvent(int** playingField, int playingFieldLines, int playingFieldColumns, SDL_Texture* extinct_flashlight, SDL_Texture* lit_flashlight, int sizeOfDstRect) {
	//Get mouse position
	int x_currentMousePos, y_currentMousePos;
	SDL_GetMouseState(&x_currentMousePos, &y_currentMousePos);
	//find the flashlight on which the mouse
	if ((x_currentMousePos / sizeOfDstRect != x_oldMousePos || y_currentMousePos / sizeOfDstRect != y_oldMousePos) && x_currentMousePos != 0 && y_currentMousePos != 0 && (x_currentMousePos < (playingFieldColumns * sizeOfDstRect)) && (y_currentMousePos < (playingFieldLines * sizeOfDstRect))) {

		x_currentMousePos = x_currentMousePos / sizeOfDstRect;
		y_currentMousePos = y_currentMousePos / sizeOfDstRect;

		if (playingField[y_currentMousePos][x_currentMousePos]) {
			//draw extinct flashlight
			loadFromFile(x_currentMousePos * sizeOfDstRect, y_currentMousePos * sizeOfDstRect, extinct_flashlight, gRenderer, sizeOfDstRect, sizeOfDstRect);
			playingField[y_currentMousePos][x_currentMousePos] = 0;
		}
		else {
			//draw lit flashlight
			loadFromFile(x_currentMousePos * sizeOfDstRect, y_currentMousePos * sizeOfDstRect, lit_flashlight, gRenderer, sizeOfDstRect, sizeOfDstRect);
			playingField[y_currentMousePos][x_currentMousePos] = 1;
		}
		//Update screen
		SDL_RenderPresent(gRenderer);
		x_oldMousePos = x_currentMousePos;
		y_oldMousePos = y_currentMousePos;
	}
}

int checkMouseLocation(int xButtonPos, int yButtonPos, int widhtButton, int heightButton) {
	int inside = 1;
	int x_curMousePos, y_curMousePos;
	SDL_GetMouseState(&x_curMousePos, &y_curMousePos);

	//Mouse is left of the button
	if (x_curMousePos < xButtonPos)
	{
		inside = 0;
	}
	//Mouse is right of the button
	else if (x_curMousePos > xButtonPos + widhtButton)
	{
		inside = 0;
	}
	//Mouse above the button
	else if (y_curMousePos < yButtonPos)
	{
		inside = 0;
	}
	//Mouse below the button
	else if (y_curMousePos > yButtonPos + heightButton)
	{
		inside = 0;
	}
	return inside;
}

int checkPlayingFieldArray(int** playingField, int  playingFieldLines, int playingFieldColumns) {
	for (int i = 0; i < playingFieldLines; i++) {
		for (int j = 0; j < playingFieldColumns; j++) {
			if (!playingField[i][j]) {
				return 0;
			}
		}
	}
	return 1;
}

void freeGamingField(int** gamingField, int numberColumns)
{
	for (int i = 0; i < numberColumns; i++) {
		free(gamingField[i]);
	}
	free(gamingField);
}

void close()
{
	//Destroy window    
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
}

void freeTexture(SDL_Texture* texture) {
	SDL_DestroyTexture(texture);
	texture = NULL;
}

SDL_Texture* loadTexture(std::string path) {
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}

int main(int argc, char* args[]) {

	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else {
		//-------------------FLAGS OF CYCLES------------------------
		// loop flags
		int  again, game = 1, start = 0;

		//game state flag
		int gameState = 0;
		int click, pause = 1;

		// flag for initializing of  timer
		int level = 0;
		//-----------------------MUSIC-----------------------------------
		//Load music
		Mix_Music* gMusic = Mix_LoadMUS("Suite dreams2.mp3");//Sunny_Side_Up.mp3
		int musicOn = 1, stopMusic;
		//-----------------------TEXT------------------------------------
		int wTextCongratul = 200, hTextCongratul = 200;
		int xTextCongratul = SCREEN_WIDTH / 2 - wTextCongratul / 2, yTextCongratul = SCREEN_HEIGHT / 2 - hTextCongratul / 2;

		//-----------------------TIMER-----------------------------------
		time_t startTime, currentTime;
		//timer location
		int xCoordTensSecond = 597, yCoordTensSecond = 15;
		//size of number
		int widhtNumberTexture = 60, heightNumberTexture = 100;
		//-----------------------GAMING FIELD------------------------------
		//create playing field array

		int sizeOfDstRect = 0, numberOfFlashlightInitLit = 0;
		//size of array of game
		int playingFieldLines = 0, playingFieldColumns = 0;

		//for random
		int  timer, xRandom, yRandom;
		srand(time(NULL));
		//-----------------BUTTONS AND TEXTURES--------------------------
		//current display texture
		SDL_Texture* background = loadTexture("background4.jpg");

		//extinct flashlight texture
		SDL_Texture* extinct_flashlight = loadTexture("extinct_flashlight2.jpg");

		//lit flashlight texture
		SDL_Texture* lit_flashlight = loadTexture("lit_flashlight2.jpg");

		SDL_Texture* arrayNumberTexture[10] = { loadTexture("zero2.jpg"),
												loadTexture("one2.jpg"),
												loadTexture("two2.jpg"),
												loadTexture("three2.jpg"),
												loadTexture("four2.jpg"),
												loadTexture("five2.jpg"),
												loadTexture("six2.jpg"),
												loadTexture("seven2.jpg"),
												loadTexture("eight2.jpg"),
												loadTexture("nine2.jpg") };
		//button "RESTART"
		SDL_Texture* restartTexture = loadTexture("restartGame.jpg");
		int wRestart = 350, hRestart = 80;
		int xRestart = (SCREEN_WIDTH / 2 - wRestart / 2), yRestart = (SCREEN_HEIGHT / 2 + hTextCongratul / 2 + 50);

		//button level "EASY"
		SDL_Texture * levelEasy = loadTexture("levelEasy.jpg");
		int wLevelEasy = 350, hLevelEasy = 80;
		int xLevelEasy = SCREEN_WIDTH / 2 - wLevelEasy / 2, yLevelEasy = 140;

		//button level "MEDIUM"
		SDL_Texture * levelMedium = loadTexture("levelMedium.jpg");
		int yLevelMedium = yLevelEasy + hLevelEasy + 20;

		//button level "HARD"
		SDL_Texture * levelHard = loadTexture("levelHard.jpg");
		int yLevelHard = yLevelMedium + hLevelEasy + 20;

		//button "RULES"
		SDL_Texture * rulesButton = loadTexture("rules.jpg");
		int yRules = yLevelHard + hLevelEasy + 20;

		//text rules
		SDL_Texture * rules = loadTexture("rules.png");
		int wTextRul = 500, hTextRul = 350;
		int xTextRul = SCREEN_WIDTH / 2 - wTextRul / 2, yTextRul = SCREEN_HEIGHT / 2 - hTextRul / 2;

		//button "OK"
		SDL_Texture * okey = loadTexture("ok2.jpg");
		int wOkey = 100, hOkey = 70;
		int xOkey = xTextRul + wTextRul / 2 - wOkey / 2, yOkey = yTextRul + hTextRul - hOkey - 10;

		//button "PAUSE"
		SDL_Texture * pauseTex = loadTexture("pause5.png");
		int xPauseTex = 570, yPauseTex = yCoordTensSecond + heightNumberTexture + 10;
		int wPauseTex = 85, hPauseTex = 85;

		//picture in pause
		SDL_Texture * picture = loadTexture("picture2.jpg");
		int xPicture = 0, yPicture = 0;
		int wPicture = SCREEN_HEIGHT, hPicture = wPicture;

		//button "RESUME"
		SDL_Texture * resume = loadTexture("unpause5.png");

		//button "SOUND"
		SDL_Texture * sound = loadTexture("sound3.jpg");
		int xSound = SCREEN_WIDTH - 150, ySound = SCREEN_HEIGHT - 150;
		int wSound = 100, hSound = 100;

		//button "UNSOUND"
		SDL_Texture * unsound = loadTexture("unsound2.png");

		//button "RESTART" in game
		SDL_Texture * restartGame = loadTexture("restart3.png");
		int xRestartGame = 665, yRestartGame = yPauseTex;
		int wRestartGame = 85, hRestartGame = 85;

		//button "MENU"
		SDL_Texture * menu = loadTexture("menu3.jpg");
		int xMenu = 575, yMenu = 450;
		int wMenu = 170, hMenu = 60;

		//Event handler
		SDL_Event e;

		//Play the music
		Mix_PlayMusic(gMusic, -1);

		//While application is running
		while (game) {

			//Load background
			SDL_RenderCopy(gRenderer, background, NULL, NULL);
			SDL_RenderPresent(gRenderer);

			//start of game
			while (gameState == 0) {
				//while
				start = 0;
				while (start == 0) {
					//BUTTON LEVEL "EASY"
					loadFromFile(xLevelEasy, yLevelEasy, levelEasy, gRenderer, wLevelEasy, hLevelEasy);

					//BUTTON LEVEL "MEDIUM"
					loadFromFile(xLevelEasy, yLevelMedium, levelMedium, gRenderer, wLevelEasy, hLevelEasy);

					//BUTTON LEVEL "HARD"
					loadFromFile(xLevelEasy, yLevelHard, levelHard, gRenderer, wLevelEasy, hLevelEasy);

					//RULES BUTTON
					loadFromFile(xLevelEasy, yRules, rulesButton, gRenderer, wLevelEasy, hLevelEasy);

					//SOUND BUTTON
					if (musicOn) {
						loadFromFile(xSound, ySound, sound, gRenderer, wSound, hSound);
					}
					else {
						loadFromFile(xSound, ySound, unsound, gRenderer, wSound, hSound);
					}
					SDL_RenderPresent(gRenderer);
					start = 1;
				}
				click = 1;
				while (start == 1) {
					while (SDL_PollEvent(&e) != 0) {

						//User requests quit
						if (e.type == SDL_QUIT)
						{
							gameState = 4;
							game = 0;
							start = 2;

						}
						//if button level "EASY" was pressed
						if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xLevelEasy, yLevelEasy, wLevelEasy, hLevelEasy)) {
							sizeOfDstRect = 112;
							numberOfFlashlightInitLit = 6;
							playingFieldLines = 5;
							playingFieldColumns = 5;
							level = 1;
							gameState = 1;
							start = 2;
						}
						//if button level "MEDIUM" was pressed
						if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xLevelEasy, yLevelMedium, wLevelEasy, hLevelEasy)) {
							sizeOfDstRect = 80, timer = 40, numberOfFlashlightInitLit = 7;
							playingFieldLines = 7;
							playingFieldColumns = 7;
							level = 2;
							gameState = 1;
							start = 2;
						}
						//if button level "HARD" was pressed
						if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xLevelEasy, yLevelHard, wLevelEasy, hLevelEasy)) {
							sizeOfDstRect = 70, timer = 30, numberOfFlashlightInitLit = 17;
							playingFieldLines = 8;
							playingFieldColumns = 8;
							level = 3;
							gameState = 1;
							start = 2;
						}
						//if button "SOUND" was pressed
						if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xSound, ySound, wSound, hSound)) {
							if (musicOn) {
								//Stop the music
								Mix_HaltMusic();
								loadFromFile(xSound, ySound, unsound, gRenderer, wSound, hSound);
							}
							else {
								loadFromFile(xSound, ySound, sound, gRenderer, wSound, hSound);
								//Play the music
								Mix_PlayMusic(gMusic, -1);
							}
							musicOn = !musicOn;
							SDL_RenderPresent(gRenderer);

						}
						//if button "RULES" was pressed
						if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xLevelEasy, yRules, wLevelEasy, hLevelEasy)) {
							//Load background
							SDL_RenderCopy(gRenderer, background, NULL, NULL);
							//draw rules
							loadFromFile(xTextRul, yTextRul, rules, gRenderer, wTextRul, hTextRul);
							loadFromFile(xOkey, yOkey, okey, gRenderer, wOkey, hOkey);
							SDL_RenderPresent(gRenderer);
							while (click) {
								while (SDL_PollEvent(&e) != 0) {
									if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xOkey, yOkey, wOkey, hOkey)) {
										//out of cyrcle
										click = 0;
										start = 0;
										//Load background
										SDL_RenderCopy(gRenderer, background, NULL, NULL);
										SDL_RenderPresent(gRenderer);
									}
									if (e.type == SDL_QUIT)
									{
										gameState = 3;
										again = 1;
										game = 0;
										click = 0;

									}
								}
							}
						}
					}
				}
			}
			//----------------------------------draw gaming field---------------------
			int** playingField = (int**)malloc(playingFieldLines * sizeof(int*));
			for (int i = 0; i < playingFieldLines; i++) {
				playingField[i] = (int*)malloc(playingFieldColumns * sizeof(int));
			}
			//INITIALIZE TIMER
			if (level == 1) {
				timer = 20;
			}
			if (level == 2) {
				timer = 35;
			}
			if (level == 3) {
				timer = 30;
			}
			while (gameState == 1) {
				//give time of game start
				time(&startTime);

				//Render texture to screen
				SDL_RenderCopy(gRenderer, background, NULL, NULL);

				//Update screen
				SDL_RenderPresent(gRenderer);

				//draw start time
				loadFromFile(xCoordTensSecond, yCoordTensSecond, arrayNumberTexture[timer / 10], gRenderer, widhtNumberTexture, heightNumberTexture);
				loadFromFile(xCoordTensSecond + widhtNumberTexture + 5, yCoordTensSecond, arrayNumberTexture[timer % 10], gRenderer, widhtNumberTexture, heightNumberTexture);
				SDL_RenderPresent(gRenderer);

				//button "PAUSE"
				loadFromFile(xPauseTex, yPauseTex, pauseTex, gRenderer, wPauseTex, hPauseTex);
				SDL_RenderPresent(gRenderer);

				//button "RESTART" in game
				loadFromFile(xRestartGame, yRestartGame, restartGame, gRenderer, wRestartGame, hRestartGame);
				SDL_RenderPresent(gRenderer);

				//buttomn "MENU"
				loadFromFile(xMenu, yMenu, menu, gRenderer, wMenu, hMenu);
				SDL_RenderPresent(gRenderer);

				//create playing field
				for (int i = 0; i < playingFieldLines; i++) {
					for (int j = 0; j < playingFieldColumns; j++) {
						//initialize playing field array
						playingField[i][j] = 0;
						loadFromFile(j * sizeOfDstRect, i * sizeOfDstRect, extinct_flashlight, gRenderer, sizeOfDstRect, sizeOfDstRect);
						//Update screen
						SDL_RenderPresent(gRenderer);
					}
				}

				//randomly light a few flashlights
				for (int i = 0; i < numberOfFlashlightInitLit; i++) {
					xRandom = rand() % playingFieldColumns;
					yRandom = rand() % playingFieldLines;

					//if not lit
					if (!playingField[yRandom][xRandom]) {
						//light in the playing field array
						playingField[yRandom][xRandom] = 1;

						//draw a lit flashlight
						loadFromFile(xRandom * sizeOfDstRect, yRandom * sizeOfDstRect, lit_flashlight, gRenderer, sizeOfDstRect, sizeOfDstRect);

						//Update screen
						SDL_RenderPresent(gRenderer);
					}
					else {
						i--;
					}
				}
				gameState = 2;
			}
			stopMusic = 0;
			//--------------------------------------------------------------------

			//game loop
			while (gameState == 2) {

				//timer
				time(&currentTime);

				if (difftime(currentTime, startTime) >= 1 && timer != 0) {
					timer--;
					loadFromFile(xCoordTensSecond, yCoordTensSecond, arrayNumberTexture[timer / 10], gRenderer, widhtNumberTexture, heightNumberTexture);
					loadFromFile(xCoordTensSecond + widhtNumberTexture + 5, yCoordTensSecond, arrayNumberTexture[timer % 10], gRenderer, widhtNumberTexture, heightNumberTexture);
					SDL_RenderPresent(gRenderer);
					time(&startTime);
				}

				while (SDL_PollEvent(&e) != 0) {
					//if button "PAUSE" pressed
					if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xPauseTex, yPauseTex, wPauseTex, hPauseTex)) {
						if (Mix_PlayingMusic()) {
							//Pause the music
							Mix_PauseMusic();
							stopMusic = 1;

						}
						pause = 1;
						loadFromFile(xPauseTex, yPauseTex, resume, gRenderer, wPauseTex, hPauseTex);
						loadFromFile(xPicture, yPicture, picture, gRenderer, wPicture, hPicture);
						SDL_RenderPresent(gRenderer);
						while (pause) {
							time(&currentTime);
							while (SDL_PollEvent(&e) != 0) {
								if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xPauseTex, yPauseTex, wPauseTex, hPauseTex)) {
									pause = 0;
									//Resume the music
									Mix_ResumeMusic();
									stopMusic = 0;
									//musicOn = 1;
								}
								if (e.type == SDL_QUIT) {
									gameState = 4;
									pause = 0;
									game = 0;
								}
								//if button "MENU" was pressed
								if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xMenu, yMenu, wMenu, hMenu)) {
									gameState = 0;
									pause = 0;
									if (musicOn) {
										//Resume the music
										Mix_ResumeMusic();
									}
									stopMusic = 0;
								}
								//if button "RESTART" in game was pressed
								if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xRestartGame, yRestartGame, wRestartGame, hRestartGame)) {
									gameState = 1;
									pause = 0;
									if (musicOn) {
										//Resume the music
										Mix_ResumeMusic();
									}
									stopMusic = 0;
								}
							}
							startTime = currentTime;
						}

						for (int i = 0; i < playingFieldLines; i++) {
							for (int j = 0; j < playingFieldColumns; j++) {
								if (!playingField[i][j]) {
									//draw extinct flashlight
									loadFromFile(j * sizeOfDstRect, i * sizeOfDstRect, extinct_flashlight, gRenderer, sizeOfDstRect, sizeOfDstRect);
								}
								else {
									//draw lit flashlight
									loadFromFile(j * sizeOfDstRect, i * sizeOfDstRect, lit_flashlight, gRenderer, sizeOfDstRect, sizeOfDstRect);
								}
							}
						}
						loadFromFile(xPauseTex, yPauseTex, pauseTex, gRenderer, wPauseTex, hPauseTex);
						SDL_RenderPresent(gRenderer);
					}
					//Handle events on queue
					if (e.type == SDL_QUIT) {

						//User requests quit
						gameState = 4;
						game = 0;
					}
					//if button "RESTART" in game was pressed
					if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xRestartGame, yRestartGame, wRestartGame, hRestartGame)) {
						gameState = 1;

					}
					//if button "MENU" was pressed
					if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xMenu, yMenu, wMenu, hMenu)) {
						gameState = 0;
					}
				}
				//game
				handleMouseEvent(playingField, playingFieldLines, playingFieldColumns, extinct_flashlight, lit_flashlight, sizeOfDstRect);

				// end of game
				if (checkPlayingFieldArray(playingField, playingFieldLines, playingFieldColumns) || timer == 0) {
					gameState = 3;
				}

			}
			//end of game (victory or fail)
			while (gameState == 3) {

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0, 128, 128, 0xFF);
				SDL_RenderClear(gRenderer);
				//write congratulations
				SDL_Color textColor = { 80, 0, 0 };
				SDL_Texture* textTexture = NULL;
				if (!timer) {
					loadText("font3.ttf", 72, "LOSE!!!", textColor, textTexture, xTextCongratul, yTextCongratul, wTextCongratul, hTextCongratul);
				}
				//write fail 
				else {
					loadText("font3.ttf", 72, "WINNER!!!", textColor, textTexture, xTextCongratul - 50, yTextCongratul, wTextCongratul + 80, hTextCongratul);
				}
				//draw button "RESTART"
				loadFromFile(xRestart, yRestart, restartTexture, gRenderer, wRestart, hRestart);
				//Update screen
				SDL_RenderPresent(gRenderer);
				again = 0;
				//restart game or not
				while (!again) {
					while (SDL_PollEvent(&e) != 0) {

						//User requests quit
						if (e.type == SDL_QUIT)
						{
							gameState = 4;
							again = 1;
							game = 0;

						}
						if (e.type == SDL_MOUSEBUTTONDOWN && checkMouseLocation(xRestart, yRestart, wRestart, hRestart)) {

							SDL_RenderPresent(gRenderer);
							gameState = 1;
							again = 1;

						}
					}
				}
				SDL_DestroyTexture(textTexture);
			}
			//free dynamic array
			freeGamingField(playingField, playingFieldColumns);
		}
		//Free resources and close SDL
		freeTexture(restartTexture);
		freeTexture(background);
		freeTexture(extinct_flashlight);
		freeTexture(lit_flashlight);
		freeTexture(levelEasy);
		freeTexture(levelMedium);
		freeTexture(levelHard);
		freeTexture(rulesButton);
		freeTexture(rules);
		freeTexture(okey);
		freeTexture(pauseTex);
		freeTexture(resume);
		freeTexture(sound);
		freeTexture(unsound);
		freeTexture(restartGame);
		freeTexture(menu);
		freeTexture(picture);
		for (int i = 0; i < 10;i++) {
			freeTexture(arrayNumberTexture[i]);
		}
		//Free the music
		Mix_FreeMusic(gMusic);
		gMusic = NULL;

		close();
	}
	return 0;
}