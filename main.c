//includes
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "wii_user.h"
#include "glcd.h"
#include "Standard5x7.h"
#include "glcd.h"
#include "Standard5x7.h"
#include "sdcard.h"
#include "mp3.h"
#include "rand.h"
#include "common.h"
#include "spi.h"
#include "adc.h"
#include "game.h"
#include "mac.h"

//variables
static uint32_t offset = 0;
static uint16_t currentGamePoints = 0;
static uint8_t volume = 255;
static uint8_t levelRefresh = 0;
static uint8_t counter = 0;
static uint8_t difficulty = 4;
static sdcard_block_t buffer;
static bool dataRequested = false;
static bool adcModeMusic = true;
static bool newValue = true;
static bool connected = false;
static bool gameStarted = false;
static bool highscoreMode = false;
static bool y_ball_bool = false;
static bool gameInitialised = false;
static bool updatedHighscoreTable = false;
static bool displayedHighscore = false;

//callback function für mp3
void requestCallback (void) {
	dataRequested = true;
}

//connection callback
void connectionCallback(uint8_t wii, connection_status_t status) {
	if (status == false) {
		//if disconnected
		connected = false;
		glcdFillScreen(0);
		wiiUserConnect(wii, mac, &connectionCallback);
	} else {
		//status == connected
		connected = true;
		while (wiiUserSetAccel(WIIUSER, true, NULL) != SUCCESS) ;
	}
}

//init function
int init(void) {
	//uses timer3
	initADC();
	spiInit();
	while (sdcardInit() != SUCCESS) ;
	mp3Init(&requestCallback);
	setPoints();
	glcdInit();
	glcdFillScreen(0);
	while (wiiUserInit(&buttonCallback, &acceleratorCallback) != SUCCESS) ;
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	return 0;
}

//play music
void playMoreMusic(void) {
	sdcardReadBlock(ADDRESS + offset, buffer);
	offset += 32;
	if (offset >= LENGTH) offset = 0;
	mp3SendMusic(buffer);
}

//gameInitialisation
void gameInit(void) {
	gameInitObjects();
	currentGamePoints = 0;
	difficulty = 4;
	updatedHighscoreTable = false;
}

//main function
int main(void) {
	
	sei();
	
	//initialisation
	init();
	
	while (true) {
		
		//setting the right volume
		if (newValue) {
			mp3SetVolume(linear(volume));
			newValue = false;
		}
		
		//play music
		if (dataRequested) {
			if (mp3Busy()) {
				dataRequested = false;
			} else {
				playMoreMusic();
			}
		}
		
		//game part
		if (counter == 0) {
			
			//generate a frame every (speed+1)*5ms
			if (!connected) {
				displayStartScreen();
				wiiUserConnect(WIIUSER, mac, &connectionCallback);
			} else {
				if (!gameStarted) {
					displayGameStart();
					if (BIT(getButt(), A_BUTTON)) {
						//set game started flag and wii led 1
						gameStarted = true;
						wiiUserSetLeds(WIIUSER, 1, NULL);
					}
				} else {
					
					//game started ...
					if (highscoreMode) {
						//update table
						if (!updatedHighscoreTable) {
							updateHighscoreTable(currentGamePoints);
							updatedHighscoreTable = true;
						}
						//display high score
						if (!displayedHighscore) {
							displayHighscore(currentGamePoints);
							displayHighscore(currentGamePoints);
							displayedHighscore = true;
						}
						if (BIT(getButt(), A_BUTTON)) {
							//set game started flag
							highscoreMode = false;
							displayedHighscore = false;
						}
					} else {
						
						//init game
						if (!gameInitialised) {
							gameInit();
							gameInitialised = true;
						}
						
						//setting the difficulty
						if (currentGamePoints >= BORDER0 && currentGamePoints < BORDER1) {
							difficulty = 3;
						}
						if (currentGamePoints >= BORDER1 && currentGamePoints < BORDER2) {
							difficulty = 2;
						}
						if (currentGamePoints >= BORDER2) {
							difficulty = 1;
						}
						
						//calculate x_ballmovement
						x_movement_ball();
					
						//calculate y_ballmovement
						//one step down every second frame, if no collision active
						if (!get_y_collision() && y_ball_bool) {
							if (getBall()->y < DISPLAY_HEIGHT-2) getBall()->y++;
							if (getBall()->y == DISPLAY_HEIGHT) getBall()->y = 0;
						}
						//every second frame
						y_ball_bool = !y_ball_bool;
						//check collision after ball movement 
						y_checkCollision();
						//if collision is active, ball is pressed upwards
						if (get_y_collision() && (levelRefresh == 0)) {
							getBall()->y--;
							if (getBall()->y <= BALL_SIZE-1) {
								//GAME OVER
								highscoreMode = true;
								gameInitialised = false;
							}
						}
					
						//update level and currentGamePoints every nth frame
						if (levelRefresh == 0) {
							updateLevel();
							currentGamePoints++;
							//check collision after platform movement 
							y_checkCollision();
							x_checkCollision();
						}
						levelRefresh++;
						if (levelRefresh >= difficulty) levelRefresh = 0;
						
						//print level
						printAll();
					}
				}
			}
		}
		if (counter >= SPEED) counter = 0;
		
		//entering power saving mode
		//sleep_cpu();
	}
	
	return 0;
}
	

//timer interrupt to start conversion
ISR(TIMER3_COMPA_vect)
{	
	//inverting mode
	adcModeMusic = !adcModeMusic;
	//switching channel
	if (adcModeMusic) {
		ADMUX &= ~(1 << MUX3 | 1 << MUX2 | 1 << MUX1 | 1 << MUX0);
	} else {
		ADMUX |= (1 << MUX3 | 1 << MUX2 | 1 << MUX1 | 1 << MUX0);
	}
	//start a conversion
	ADCSRA |= 1 << ADSC;
	//updating counter for game, reducing timer interrupts
	counter++;
}

//read ADC output and seed random number generator
ISR(ADC_vect)
{
	if (adcModeMusic) {
		//setting volume
		volume = ADC >> 2;
		newValue = true;
	} else {
		//seeding
		rand_feed(BIT(ADC,0));
	}
}


