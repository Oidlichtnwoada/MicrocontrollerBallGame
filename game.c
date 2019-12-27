//imports
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "wii_user.h"
#include "glcd.h"
#include "Standard5x7.h"
#include "common.h"
#include "rand.h"

//some variables
static uint16_t highscoreArray[HIGHSCORECOUNT] = {0,0,0,0,0};
static uint16_t butt = 0;
static uint8_t X = 0;
char highscoreCharArr[HIGHSCORECOUNT][12];
const char startScreen[] PROGMEM = "Press A to connect!";
const char gameStart[] PROGMEM = "Press A to\nstart game!";
const char gameStartHighScore[] PROGMEM = "Press A\nto start\ngame!";
static bool y_collision = false;
static bool x_collision_left = false;
static bool x_collision_right = false;
static struct xy_point_t left, middle, first, ball, highScoreGameStart;

//getter
uint16_t getButt(void) {
	return butt;
}
struct xy_point_t *getBall(void) {
	return &ball;
}
bool get_y_collision(void) {
	return y_collision;
}

//wii mote callbacks
void buttonCallback(uint8_t wii, uint16_t buttonStates) {
	butt = buttonStates;
}
void acceleratorCallback(uint8_t wii, uint16_t x, uint16_t y, uint16_t z) {
	X = x >> 2;
}

//initialise points
void setPoints(void) {
	left.x = 10;
	left.y = 36;
	middle.x = 32;
	middle.y = 32;
	first.x = 5;
	first.y = 15;
	highScoreGameStart.x = 70;
	highScoreGameStart.y = 28;
}

//display methods
void displayStartScreen(void) {
	glcdFillScreen(0);
	glcdDrawTextPgm(startScreen, left, &Standard5x7, &glcdSetPixel);
}
void displayGameStart(void) {
	glcdFillScreen(0);
	glcdDrawTextPgm(gameStart, middle, &Standard5x7, &glcdSetPixel);
}
void displayHighscore(uint16_t cgp) {
	glcdFillScreen(0);
	for (uint8_t i = 0; i < HIGHSCORECOUNT; i++) {
		first.y += 10;
		struct xy_point_t temp = first;
		temp.x = first.x + 58;
		temp.y = first.y - 8; 
		sprintf(highscoreCharArr[i], "%d: %i", i+1 , highscoreArray[i]);
		glcdDrawText(highscoreCharArr[i], first, &Standard5x7, &glcdSetPixel);
		first.x--;	
		if (cgp == highscoreArray[i]) {
			glcdFillRect(first, temp, &glcdInvertPixel);
		}
		first.x++;
	}
	first.y = 5;
	glcdDrawTextPgm(gameStartHighScore, highScoreGameStart, &Standard5x7, &glcdSetPixel);
}

//updating highscore table
void updateHighscoreTable(uint16_t cgp) {
	bool writtenToTable = false;
	uint16_t temp;
	uint16_t storage;
	for (int i = 0; i < HIGHSCORECOUNT; i++) {
		if ((highscoreArray[i] < cgp) & !writtenToTable) {
			temp = highscoreArray[i];
			highscoreArray[i] = cgp;
			writtenToTable = true;
		} else {
			if (writtenToTable) {
				storage = highscoreArray[i];
				highscoreArray[i] = temp;
				temp = storage;
			}
		}
	}
}

//my level struct
struct LineWithHoles {
	uint8_t y_start;
	uint8_t begin1;
	uint8_t length1;
	uint8_t begin2;
	uint8_t length2;
	uint8_t begin3;
	uint8_t length3;
};

//declaring my level
static struct LineWithHoles level[PLATFORM_COUNT];

//randomises platform with adc noise input
void randomisePlatform(struct LineWithHoles *platform) {
	do {
		platform->begin1 = rand16() >> 12;
		platform->length1 = rand16() >> 10; 
		platform->begin2 = platform->begin1 + platform->length1 + BALL_SIZE * GAP_SIZE;
		platform->length2 = rand16() >> 11;
		platform->begin3 = platform->begin2 + platform->length2 + BALL_SIZE * GAP_SIZE;
		platform->length3 = rand16() >> 11;
	} while (platform->begin3 + platform->length3 > DISPLAY_WIDTH-1 || platform->begin3 + platform->length3 < DISPLAY_WIDTH-BALL_SIZE*GAP_SIZE);
}

//updating platform and randomise them if necessary
void updatePlatform(struct LineWithHoles *platform) {
	if (platform->y_start == 0) randomisePlatform(platform);
	platform->y_start--;
	if (platform->y_start == UINTMAX8) {
		platform->y_start = DISPLAY_HEIGHT-1;
		
	}
}

//shifts the whole level upwards
void updateLevel(void) {
	for (uint8_t i = 0; i < PLATFORM_COUNT; i++) {
		updatePlatform(&level[i]);
	}
}

//print functions
void printBall(void) {
	struct xy_point_t temp = ball;
	struct xy_point_t end = ball;
	end.x += BALL_SIZE-1;
	for (uint8_t i = 0; i < BALL_SIZE; i++) {
		if ((i == 0) | (i == BALL_SIZE-1)) {ball.x++; end.x--;}
		glcdDrawLine(ball, end, &glcdSetPixel);
		ball.y--;
		end.y--;
		if ((i == 0) | (i == BALL_SIZE-1)) {ball.x--; end.x++;}
	}
	ball = temp;
}
void printPlatform(struct LineWithHoles *platform) {
	//allocating space for points
	struct xy_point_t tempStart;
	struct xy_point_t tempEnd;
	//y stays the same
	tempStart.y = platform->y_start;
	tempEnd.y = platform->y_start;
	//printing the three platforms
	tempStart.x = platform->begin1;
	tempEnd.x = platform->begin1 + platform->length1;
	glcdDrawLine(tempStart, tempEnd, &glcdSetPixel);
	tempStart.x = platform->begin2;
	tempEnd.x = platform->begin2 + platform->length2;
	glcdDrawLine(tempStart, tempEnd, &glcdSetPixel);
	tempStart.x = platform->begin3;
	tempEnd.x = platform->begin3 + platform->length3;
	glcdDrawLine(tempStart, tempEnd, &glcdSetPixel);
}
void printLevel(void) {
	for (uint8_t i = 0; i < PLATFORM_COUNT; i++) {
		printPlatform(&level[i]);
	}
}
void printAll(void) {
	glcdFillScreen(0);
	printBall();
	printLevel();
}

// x collision detection and movement
void x_checkCollision(void) {
	
	//setting defaults
	x_collision_right = false;
	x_collision_left = false;
	
	for (uint8_t i = 0; i < PLATFORM_COUNT; i++) {
		
		//check if platform is in suitable range
		if (level[i].y_start > ball.y || level[i].y_start < ball.y - BALL_SIZE + 1) continue;
		
		if (level[i].begin1 - 1 == ball.x + BALL_SIZE-1) {
			x_collision_right = true;
		}
		if (level[i].begin1 + level[i].length1 + 1 == ball.x) {
			x_collision_left = true;
		}
		if (level[i].begin2 - 1 == ball.x + BALL_SIZE-1) {
			x_collision_right = true;
		}
		if (level[i].begin2 + level[i].length2 + 1 == ball.x) {
			x_collision_left = true;
		}
		if (level[i].begin3 - 1 == ball.x + BALL_SIZE-1) {
			x_collision_right = true;
		}
		if (level[i].begin3 + level[i].length3 + 1 == ball.x) {
			x_collision_left = true;
		}
	}
	return;
}
void x_movement_ball(void) {
	//calculate x_movement
	int8_t mov = (X-128) >> 3;
	if (mov >= 0) {
		while (mov != 0 && !x_collision_right) {
			mov--;
			if (ball.x != DISPLAY_WIDTH-BALL_SIZE) ball.x++;
			x_checkCollision();
		}
	}
	if (mov < 0) {
		while (mov != 0 && !x_collision_left) {
			mov++;
			if (ball.x != 0) ball.x--;
			x_checkCollision();
		}
	}
}

// y collision detection
void y_checkCollision(void) {
	for (uint8_t i = 0; i < PLATFORM_COUNT; i++) {
		if (level[i].y_start - 1 != ball.y) continue;
		if (level[i].begin1 <= ball.x + BALL_SIZE-1 && level[i].begin1 + level[i].length1 >= ball.x) {y_collision = true; return;}
		if (level[i].begin2 <= ball.x + BALL_SIZE-1 && level[i].begin2 + level[i].length2 >= ball.x) {y_collision = true; return;}
		if (level[i].begin3 <= ball.x + BALL_SIZE-1 && level[i].begin3 + level[i].length3 >= ball.x) {y_collision = true; return;}
	}
	y_collision = false;
	return;
}

//initialise game objects to start a new game
void gameInitObjects(void) {
	for (uint8_t i = 0; i < PLATFORM_COUNT; i++) {
		level[i].y_start = 5 + i * (DISPLAY_HEIGHT/PLATFORM_COUNT);
		level[i].begin1 = 10;
		level[i].length1 = 10;
		level[i].begin2 = 25;
		level[i].length2 = 15;
		level[i].begin3 = 110;
		level[i].length3 = 5;
		ball.x = DISPLAY_WIDTH/2;
		ball.y = 10;
	}
}







