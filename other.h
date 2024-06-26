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
extern const unsigned char CHAR_SNAKE_BODY_R_D[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_BODY_R_U[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_BODY_L_U[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_BODY_L_D[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_HEAD_OPEN_LEFT[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_HEAD_OPEN_UP[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_HEAD_OPEN_RIGHT[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_HEAD_OPEN_DOWN[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_BODY_DBL_UP[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_BODY_DBL_LEFT[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_BODY_DBL_DOWN[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_BODY_DBL_RIGHT[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_TAIL_UP_BODY_DOWN[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_TAIL_LEFT_BODY_RIGHT[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_TAIL_DOWN_BODY_UP[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_TAIL_RIGHT_BODY_LEFT[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_BODY_HORIZONTAL[CHAR_SIZE];
extern const unsigned char CHAR_SNAKE_BODY_VERTICAL[CHAR_SIZE];
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
const unsigned char CHAR_SNAKE_BODY_R_U[CHAR_SIZE] = {0x3c, 0x3e, 0x1f, 0x1f, 0x0f, 0x03, 0x00, 0x00};
const unsigned char CHAR_SNAKE_BODY_R_D[CHAR_SIZE] = {0x00, 0x00, 0x03, 0x0f, 0x1f, 0x1f, 0x3e, 0x3c};
const unsigned char CHAR_SNAKE_BODY_L_D[CHAR_SIZE] = {0x00, 0x00, 0xc0, 0xf0, 0xf8, 0xf8, 0x7c, 0x3c};
const unsigned char CHAR_SNAKE_BODY_L_U[CHAR_SIZE] = {0x3c, 0x7c, 0xf8, 0xf8, 0xf0, 0xc0, 0x00, 0x00};
const unsigned char CHAR_SNAKE_HEAD_OPEN_LEFT[CHAR_SIZE] = {0x00, 0xfe, 0x9e, 0xf8, 0xe0, 0x80, 0xfe, 0x00}; // mouth open head left
const unsigned char CHAR_SNAKE_HEAD_OPEN_UP[CHAR_SIZE] = {0x00, 0x62, 0x62, 0x72, 0x72, 0x5a, 0x5a, 0x7e}; // mouth open head up
const unsigned char CHAR_SNAKE_HEAD_OPEN_RIGHT[CHAR_SIZE] = {0x00, 0x7f, 0x79, 0x1f, 0x07, 0x01, 0x7f, 0x00}; // mouth open head right
const unsigned char CHAR_SNAKE_HEAD_OPEN_DOWN[CHAR_SIZE] = {0x7e, 0x5a, 0x5a, 0x4e, 0x4e, 0x46, 0x46, 0x00}; // mouth open head down
const unsigned char CHAR_SNAKE_BODY_DBL_UP[CHAR_SIZE] = {0x3c, 0x7a, 0xf7, 0xef, 0xe7, 0xfe, 0x7e, 0x3c}; // double back up
const unsigned char CHAR_SNAKE_BODY_DBL_RIGHT[CHAR_SIZE] = {0x3c, 0x7e, 0xff, 0xe7, 0xeb, 0xfd, 0x7e, 0x3c}; // double back right
const unsigned char CHAR_SNAKE_BODY_DBL_DOWN[CHAR_SIZE] = {0x3c, 0x7e, 0xff, 0xe7, 0xf7, 0xef, 0x5e, 0x3c}; // double back down
const unsigned char CHAR_SNAKE_BODY_DBL_LEFT[CHAR_SIZE] = {0x3c, 0x7e, 0xbf, 0xd7, 0xe7, 0xff, 0x7e, 0x3c}; // double back left
const unsigned char CHAR_SNAKE_TAIL_DOWN_BODY_UP[CHAR_SIZE] = {0x00, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x3c}; // tail up body down
const unsigned char CHAR_SNAKE_TAIL_RIGHT_BODY_LEFT[CHAR_SIZE] = {0x00, 0x00, 0xe0, 0xfe, 0xfe, 0xe0, 0x00, 0x00}; // tail left body right
const unsigned char CHAR_SNAKE_TAIL_UP_BODY_DOWN[CHAR_SIZE] = {0x3c, 0x3c, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x00}; // tail down body up
const unsigned char CHAR_SNAKE_TAIL_LEFT_BODY_RIGHT[CHAR_SIZE] = {0x00, 0x00, 0x07, 0x7f, 0x7f, 0x07, 0x00, 0x00}; // tail right body left
const unsigned char CHAR_SNAKE_BODY_HORIZONTAL[CHAR_SIZE] = {0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00}; // body horizontal
const unsigned char CHAR_SNAKE_BODY_VERTICAL[CHAR_SIZE] = {0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c}; // body vertical
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
const unsigned char CHAR_DIGIT_0[CHAR_SIZE] = {0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00};
const unsigned char CHAR_DIGIT_1[CHAR_SIZE] = {0x08, 0x18, 0x28, 0x08, 0x08, 0x08, 0x3e, 0x00};
const unsigned char CHAR_DIGIT_2[CHAR_SIZE] = {0x3c, 0x42, 0x02, 0x0c, 0x30, 0x40, 0x7e, 0x00};
const unsigned char CHAR_DIGIT_3[CHAR_SIZE] = {0x3c, 0x42, 0x02, 0x1c, 0x02, 0x42, 0x3c, 0x00};
const unsigned char CHAR_DIGIT_4[CHAR_SIZE] = {0x04, 0x0c, 0x14, 0x24, 0x7e, 0x04, 0x04, 0x00};
const unsigned char CHAR_DIGIT_5[CHAR_SIZE] = {0x7e, 0x40, 0x78, 0x04, 0x02, 0x44, 0x38, 0x00};
const unsigned char CHAR_DIGIT_6[CHAR_SIZE] = {0x1c, 0x20, 0x40, 0x7c, 0x42, 0x42, 0x3c, 0x00};
const unsigned char CHAR_DIGIT_7[CHAR_SIZE] = {0x7e, 0x42, 0x04, 0x08, 0x10, 0x10, 0x10, 0x00};
const unsigned char CHAR_DIGIT_8[CHAR_SIZE] = {0x3c, 0x42, 0x42, 0x3c, 0x42, 0x42, 0x3c, 0x00};
const unsigned char CHAR_DIGIT_9[CHAR_SIZE] = {0x3c, 0x42, 0x42, 0x3e, 0x02, 0x04, 0x38, 0x00};
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
    void takeTurn();
    void placeOnScreen();

private:
    unsigned int bodyLen;
    int bodyHead;
    Point nextHead;
    Direction lastDirection;
    Direction nextDirection;
    ScreenChar myHead;
    BodyPart bodyPart[SNAKE_BODY_LEN];

    void updateBody();
    void makeMove();
    void generateNextPosition();
    void generateNextDirection();
};

#endif // OTHER_H