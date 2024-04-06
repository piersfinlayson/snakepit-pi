#ifndef SNAKEPIT_H
#define SNAKEPIT_H

#include <circle/logger.h>
#include <circle/screen.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbkeyboard.h>
#include <circle/timer.h>
#include <circle/bcmrandom.h>
#include "other.h"
#include "game.h"

#define DEFAULT_MOVES_PER_S 5

#ifdef GAME_CPP
int num_eggs = 0;
unsigned int moves_per_s = DEFAULT_MOVES_PER_S;
volatile char last_key_pressed = 0;
bool run = false;
bool playerEaten = false;
Point cellsChanged[MAX_CHANGED_CELLS];
unsigned int numCellsChanged = 0;
unsigned int score = 0;
unsigned int highScore = 0;
#else // GAME_CPP
extern int num_eggs;
extern unsigned int moves_per_s;
extern volatile char last_key_pressed;
extern bool run;
extern bool playerEaten;
extern Point cellsChanged[];
extern unsigned int numCellsChanged;
extern unsigned int score;
extern unsigned int highScore;
#endif // GAME_CPP

#endif // SNAKEPIT_H

