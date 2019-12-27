#ifndef game_h
#define game_h

//wii callbacks
void buttonCallback(uint8_t wii, uint16_t buttonStates);
void acceleratorCallback(uint8_t wii, uint16_t x, uint16_t y, uint16_t z);

//initialise points
void setPoints(void);

void displayStartScreen(void);



void displayHighscore(uint16_t cgp);

void updateHighscoreTable(uint16_t cgp);

struct LineWithHoles {
	uint8_t y_start;
	uint8_t begin1;
	uint8_t length1;
	uint8_t begin2;
	uint8_t length2;
	uint8_t begin3;
	uint8_t length3;
};

void randomisePlatform(struct LineWithHoles *platform);

void updatePlatform(struct LineWithHoles *platform);

void updateLevel(void);

void printBall(void);

void printPlatform(struct LineWithHoles *platform);

void printLevel(void);

void printAll(void);

void x_checkCollision(void);

void x_movement_ball(void);

void gameInit(void);

void y_checkCollision(void);

uint16_t getButt(void);

void gameInitObjects(void);

struct xy_point_t *getBall(void);

bool get_y_collision(void);

void displayGameStart(void);

#endif








