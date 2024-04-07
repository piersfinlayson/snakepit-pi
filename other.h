#ifndef OTHER_H
#define OTHER_H

#define NUM_SNAKES 7
#define CHAR_SIZE 8

// How many times bigger to make the game image
#define ZOOM_X 4
#define ZOOM_Y 3

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define COMPLETE_GAME_HEIGHT  ((2*BORDER_WIDTH) + (SNAKE_PIT_ROWS*CHAR_SIZE*ZOOM_Y) + (1*CHAR_SIZE*ZOOM_Y))
#define COMPLETE_GAME_WIDTH   ((2*BORDER_WIDTH) + (SNAKE_PIT_COLS*CHAR_SIZE*ZOOM_X))
#define GAME_OFFSET_FROM_TOP  ((SCREEN_HEIGHT - COMPLETE_GAME_HEIGHT) / 2)

// These are the offsets from the edge of the screen to top left corner of the snake pit
#define SCREEN_ROW_OFFSET  (GAME_OFFSET_FROM_TOP + BORDER_WIDTH + (1*CHAR_SIZE*ZOOM_Y))
#define SCREEN_COL_OFFSET  800

#define SNAKE_PIT_ROWS 22
#define SNAKE_PIT_COLS 22
#define MAX_CHANGED_CELLS 256 // Max number of changed cells before full redraw required

#define SCORE_BAR_COLS  SNAKE_PIT_COLS
#define MAX_SCORE_DIGITS 6
#define SCORE_BAR_ROW_OFFSET  (SCREEN_ROW_OFFSET - (1*CHAR_SIZE*ZOOM_Y))
#define SCORE_BAR_COL_OFFSET  SCREEN_COL_OFFSET
#define SCORE_EGG_INCREMENT   10
#define SCORE_SNAKE_INCREMENT 10

// Size in pixels of border around snake pit and above score bar
#define BORDER_WIDTH 100
#define TSCREEN_BORDER_COLOUR RED_COLOR

#define SNAKE_BODY_LEN 20

#define ASSERT(X)   if (!(X))                                                                   \
                    {                                                                           \
                        LOGERR("Assertion failed: " #X);                                        \
                        uintptr ulStackPtr;                                                     \
                        uintptr *pStackPtr = &ulStackPtr;                                       \
                        for (unsigned i = 0; i < 64; i++, pStackPtr++)                          \
                        {                                                                       \
                            extern unsigned char _etext;                                        \
                                                                                                \
                            if (   *pStackPtr >= MEM_KERNEL_START                               \
                                && *pStackPtr < (uintptr) &_etext                               \
                                && (*pStackPtr & 3) == 0)                                       \
                            {                                                                   \
                                LOGNOTE("stack[%u] is 0x%lX", i, (unsigned long) *pStackPtr);   \
                            }                                                                   \
                        }                                                                       \
                        assert(false);                                                          \
                    }

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
    void setDigit(unsigned int digit, bool reverse);
    void setDigit(unsigned int digit, unsigned int overrideColour, bool reverse);
    void set(ScreenChar ch, bool reverse = false);
    void set(ScreenChar ch, unsigned int overrideColour, bool reverse = false);
};

#define PLAYER_COLOUR 0xbbbbbb
#define EGG_COLOUR 0xaa0001
#define EMPTY_COLOUR 0x000000
#define SCORE_COLOUR 0xaa0000

#if 0
const uint64_t IMAGES[] = {
  0x0000030f1f1f3e3c,
  0x3c3e1f1f0f030000,
  0x3c7cf8f8f0c00000,
  0x0000c0f0f8f87c3c,
  0x00fe80e0f89efe00,
  0x7e5a5a7272626200,
  0x007f01071f797f00,
  0x0046464e4e5a5a7e,
  0x3c7effe7eff77a3c,
  0x3c7efdebe7ff7e3c,
  0x3c5eeff7e7ff7e3c,
  0x3c7effe7d7bf7e3c,
  0x3c3c3c1818181800,
  0x0000e0fefee00000,
  0x00181818183c3c3c,
  0x0000077f7f070000,
  0x0000ffffffff0000,
  0x3c3c3c3c3c3c3c3c
};
const int IMAGES_LEN = sizeof(IMAGES)/8;
#endif 

#ifdef GAME_CPP
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
const unsigned char CHAR_EGG[CHAR_SIZE] = {0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00};
const unsigned char CHAR_SNAKE_HEAD_DOWN[CHAR_SIZE] = {0x7e, 0x5e, 0x52, 0x7a, 0x3a, 0x1a, 0x1a, 0x00};
const unsigned char CHAR_SNAKE_HEAD_UP[CHAR_SIZE] = {0x00, 0x58, 0x58, 0x5c, 0x5e, 0x4a, 0x7a, 0x7e};
const unsigned char CHAR_SNAKE_HEAD_RIGHT[CHAR_SIZE] = {0x00, 0xf0, 0x98, 0xfe, 0xde, 0xc0, 0xfe, 0x00};
const unsigned char CHAR_SNAKE_HEAD_LEFT[CHAR_SIZE] = {0x00, 0x0f, 0x19, 0x7f, 0xfb, 0x03, 0x7f, 0x00};
const unsigned char CHAR_SNAKE_BODY[CHAR_SIZE] = {0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00};
const unsigned char CHAR_PLAYER_MOUTH_OPEN[CHAR_SIZE] = {0x7e, 0x99, 0xff, 0xc3, 0x81, 0xc3, 0xe7, 0x7e};
const unsigned char CHAR_PLAYER_MOUTH_CLOSED[CHAR_SIZE] = {0x7e, 0x99, 0x99, 0xff, 0xbd, 0xc3, 0xff, 0x7e};
const unsigned char CHAR_EMPTY[CHAR_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
ScreenChar sc_egg(CHAR_EGG, EGG_COLOUR);
ScreenChar sc_empty(CHAR_EMPTY, EMPTY_COLOUR);
ScreenChar sc_snake_head_down(CHAR_SNAKE_HEAD_DOWN, 0);
ScreenChar sc_snake_head_up(CHAR_SNAKE_HEAD_UP, 0);
ScreenChar sc_snake_head_right(CHAR_SNAKE_HEAD_RIGHT, 0);
ScreenChar sc_snake_head_left(CHAR_SNAKE_HEAD_LEFT, 0);
ScreenChar sc_snake_body(CHAR_SNAKE_BODY, 0);
ScreenChar sc_player_mouth_open(CHAR_PLAYER_MOUTH_OPEN, PLAYER_COLOUR);
ScreenChar sc_player_mouth_closed(CHAR_PLAYER_MOUTH_CLOSED, PLAYER_COLOUR);
ScreenChar sc_snake_pit[SNAKE_PIT_ROWS][SNAKE_PIT_COLS];
ScreenChar sc_score_bar[SCORE_BAR_COLS];
#else // GAME_CPP
extern const unsigned int SNAKE_COLOURS[NUM_SNAKES];
extern const unsigned char CHAR_EGG[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_HEAD_DOWN[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_HEAD_UP[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_HEAD_LEFT[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_HEAD_RIGHT[CHAR_SIZE];
extern const unsigned char CHAR_PLAYER_MOUTH_OPEN[CHAR_SIZE];
extern const unsigned char CHAR_PLAYER_MOUTH_CLOSED[CHAR_SIZE];
extern const unsigned char CHAR_EMPTY[CHAR_SIZE];
extern ScreenChar sc_egg;
extern ScreenChar sc_empty;
extern ScreenChar sc_snake_head_down;
extern ScreenChar sc_snake_head_up;
extern ScreenChar sc_snake_head_left;
extern ScreenChar sc_snake_head_right;
extern ScreenChar sc_snake_body;
extern ScreenChar sc_player_mouth_open;
extern ScreenChar sc_player_mouth_closed;
extern ScreenChar sc_snake_pit[SNAKE_PIT_ROWS][SNAKE_PIT_COLS];
// no extern ScreenChar sc_score_bar[SCORE_BAR_COLS];
#endif // GAME_CPP

#ifdef OTHER_CPP
const unsigned char CHAR_DIGIT_0[CHAR_SIZE] = {0x3c, 0x42, 0x46, 0x5a, 0x52, 0x62, 0x3c, 0x00};
const unsigned char CHAR_DIGIT_1[CHAR_SIZE] = {0x10, 0x30, 0x10, 0x10, 0x10, 0x10, 0x7e, 0x00};
const unsigned char CHAR_DIGIT_2[CHAR_SIZE] = {0x3c, 0x42, 0x02, 0x0c, 0x10, 0x20, 0x7e, 0x00};
const unsigned char CHAR_DIGIT_3[CHAR_SIZE] = {0x3c, 0x42, 0x02, 0x1c, 0x02, 0x42, 0x3c, 0x00};
const unsigned char CHAR_DIGIT_4[CHAR_SIZE] = {0x04, 0x0c, 0x14, 0x24, 0x7e, 0x04, 0x04, 0x00};
const unsigned char CHAR_DIGIT_5[CHAR_SIZE] = {0x7e, 0x40, 0x40, 0x7c, 0x02, 0x42, 0x3c, 0x00};
const unsigned char CHAR_DIGIT_6[CHAR_SIZE] = {0x3c, 0x42, 0x40, 0x7c, 0x42, 0x42, 0x3c, 0x00};
const unsigned char CHAR_DIGIT_7[CHAR_SIZE] = {0x7e, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00};
const unsigned char CHAR_DIGIT_8[CHAR_SIZE] = {0x3c, 0x42, 0x42, 0x3c, 0x42, 0x42, 0x3c, 0x00};
const unsigned char CHAR_DIGIT_9[CHAR_SIZE] = {0x3c, 0x42, 0x42, 0x3e, 0x02, 0x42, 0x3c, 0x00};
const ScreenChar CHAR_DIGITS[10] =
{
    {CHAR_DIGIT_0, EMPTY_COLOUR},
    {CHAR_DIGIT_1, EMPTY_COLOUR},
    {CHAR_DIGIT_2, EMPTY_COLOUR},
    {CHAR_DIGIT_3, EMPTY_COLOUR},
    {CHAR_DIGIT_4, EMPTY_COLOUR},
    {CHAR_DIGIT_5, EMPTY_COLOUR},
    {CHAR_DIGIT_6, EMPTY_COLOUR},
    {CHAR_DIGIT_7, EMPTY_COLOUR},
    {CHAR_DIGIT_8, EMPTY_COLOUR},
    {CHAR_DIGIT_9, EMPTY_COLOUR}
};
#else // OTHER_CPP
extern const unsigned char CHAR_DIGIT_0[CHAR_SIZE];
extern const unsigned char CHAR_DIGIT_1[CHAR_SIZE];
extern const unsigned char CHAR_DIGIT_2[CHAR_SIZE];
extern const unsigned char CHAR_DIGIT_3[CHAR_SIZE];
extern const unsigned char CHAR_DIGIT_4[CHAR_SIZE];
extern const unsigned char CHAR_DIGIT_5[CHAR_SIZE];
extern const unsigned char CHAR_DIGIT_6[CHAR_SIZE];
extern const unsigned char CHAR_DIGIT_7[CHAR_SIZE];
extern const unsigned char CHAR_DIGIT_8[CHAR_SIZE];
extern const unsigned char CHAR_DIGIT_9[CHAR_SIZE];
extern const ScreenChar CHAR_DIGITS[10];
#endif // OTHER_CPP

void changeCell(Point cellPos, ScreenChar ch);

enum Animation { OPEN, CLOSED };

class Player {
public:

    Point pos;
    Animation animation;

    Player(Point pos);
    void placeOnScreen();
    void takeTurn();
};

struct BodyPart
{
    ScreenChar bodyChar;
    Point pos;
};

class Snake
{
public:
    enum Master { MASTER, NOT_MASTER };
    enum Direction { UP, DOWN, LEFT, RIGHT };

    Point head;
    const Master master; // Can eat eggs
    const unsigned int colour;
    Animation animation;

    Snake();
    Snake(Point head, Master master, unsigned int colour);
    void init(Point pos);
    void placeOnScreen();
    void takeTurn();

private:
    Direction lastDirection;
    ScreenChar myHead;
    BodyPart bodyPart[SNAKE_BODY_LEN];
    unsigned int bodyLen;
    int bodyHead;

    void updateBody();
    void makeMove(Direction direction);
    Direction generateMove();
};

#endif // OTHER_H