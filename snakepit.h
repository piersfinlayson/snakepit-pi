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

#define NUM_SNAKES 7
#define CHAR_SIZE 8

#define SCREEN_ROW_OFFSET 100
#define SCREEN_COL_OFFSET 1000

#define SNAKE_PIT_ROWS 22
#define SNAKE_PIT_COLS 22

// How many times bigger to make the game image
#define ZOOM_X 4
#define ZOOM_Y 2

const unsigned int SNAKE_COLOURS[NUM_SNAKES] =
{
    0xaa0000, // red
    0xaaaaaa, // white
    0x00aaaa, // cyan
    0xaa00aa, // purple
    0x00aa00, // green
    0x0000aa, // blue
    0xaaaa00  // yellow
};
#define PLAYER_COLOUR 0xaaaaaa
#define EGG_COLOUR 0xaa5555
#define EMPTY_COLOUR 0x000000

void KeyPressedHandler(const char *string);
void KeyReleasedHandler(const char *string);
void KeyboardRemovedHandler(CDevice *device, void *context);
void KeyboardShutdownHandler();

struct Point
{
    int x;
    int y;

    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const
    {
        return x != other.x || y != other.y;
    }
};

class ScreenChar
{
public:
    unsigned char contents[CHAR_SIZE];
    unsigned int colour;

    ScreenChar();
    ScreenChar(const unsigned char contents[CHAR_SIZE], unsigned int colour);
    void set(ScreenChar ch);
    void set(ScreenChar ch, unsigned int overrideColour);
};

const unsigned char CHAR_EGG[CHAR_SIZE] = {0x3e, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3e, 0x00};
const unsigned char CHAR_SNAKE_HEAD[CHAR_SIZE] = {0x3e, 0x41, 0x55, 0x41, 0x41, 0x41, 0x3e, 0x00};
const unsigned char CHAR_SNAKE_HEAD_DOWN[CHAR_SIZE] = {0x7e, 0x5e, 0x52, 0x7a, 0x3a, 0x1a, 0x1a, 0x00};
//const unsigned char CHAR_SNAKE_HEAD_UP[CHAR_SIZE] = {0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x};
//const unsigned char CHAR_SNAKE_HEAD_LEFT[CHAR_SIZE] = {0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x};
//const unsigned char CHAR_SNAKE_HEAD_RIGHT[CHAR_SIZE] = {0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x};
const unsigned char CHAR_PLAYER_MOUTH_OPEN[CHAR_SIZE] = {0x7e, 0x99, 0xff, 0xc3, 0x81, 0xc3, 0xe7, 0x7e};
const unsigned char CHAR_PLAYER_MOUTH_CLOSED[CHAR_SIZE] = {0x7e, 0x99, 0x99, 0xff, 0xbd, 0xc3, 0xff, 0x7e};
const unsigned char CHAR_EMPTY[CHAR_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};

class Player {
public:
    enum Animation { OPEN, CLOSED };

    Point pos;
    Animation animation;

    Player(Point pos, CLogger *logger);
    void placeOnScreen();
    void takeTurn();

private:
    CLogger *logger;
};

class Snake
{
public:
    enum Master { MASTER, NOT_MASTER };
    enum Direction { UP, DOWN, LEFT, RIGHT };

    Point head;
    const Master master; // Can eat eggs
    const unsigned int colour;

    Snake();
    Snake(Point head, Master master, unsigned int colour, CLogger *logger);
    void placeOnScreen();
    void takeTurn();

private:
    Direction lastDirection;
    ScreenChar myHead;
    CLogger *logger;

    void makeMove(Direction direction);
    Direction generateMove();
};

class Game
{
public:
    Game(CDeviceNameService deviceNameService, CScreenDevice screen, CTimer timer, CLogger logger, CUSBKeyboardDevice * volatile keyboard);
    bool init();
    void go();

private:
    CDeviceNameService deviceNameService;
    CScreenDevice screen;
    CTimer timer;
    CLogger logger;
    CUSBKeyboardDevice * volatile keyboard;
    Player* player;
    Snake** snake;
    void get_keyboard();
    Player* init_player();
    Snake** init_snakes();
    void reset_player();
    void reset_snakes();
    void init_snake_pit();
    void render_snake_pit();
    void draw_char(const unsigned char *contents, unsigned int colour, unsigned char attr, int x, int y);
};

#endif // SNAKEPIT_H

