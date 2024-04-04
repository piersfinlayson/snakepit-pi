#include "snakepit.h"

#define CHAR_SIZE 8

#define SCREEN_ROW_OFFSET 100
#define SCREEN_COL_OFFSET 100

#define SNAKE_PIT_ROWS 22
#define SNAKE_PIT_COLS 22
#define NUM_SNAKES 7

const unsigned int SNAKE_COLOURS[NUM_SNAKES] =
{
    0xaa0000, // red
    0xaaaaaa, // white
    0x00aaaa, // cyan
    0xaa00aa, // pruple
    0x00aa00, // green
    0x0000aa, // blue
    0xaaaa00  // yellow
};
#define PLAYER_COLOUR 0xaaaaaa
#define EGG_COLOUR 0x00aa00
#define EMPTY_COLOUR 0x000000

const unsigned char CHAR_EGG[CHAR_SIZE] = {0x3e, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3e, 0x00};
const unsigned char CHAR_SNAKE_HEAD[CHAR_SIZE] = {0x3e, 0x41, 0x55, 0x41, 0x41, 0x41, 0x3e, 0x00};
const unsigned char CHAR_PLAYER_MOUTH_OPEN[CHAR_SIZE] = {0x3e, 0x41, 0x55, 0x49, 0x55, 0x49, 0x3e, 0x00};
const unsigned char CHAR_PLAYER_MOUTH_CLOSED[CHAR_SIZE] = {0x3e, 0x41, 0x55, 0x41, 0x5d, 0x41, 0x3e, 0x00};
const unsigned char CHAR_EMPTY[CHAR_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};

//
// ScreenChar
//
// The bitmap and colour of a character on the screen
//
class ScreenChar
{
public:
    unsigned char contents[CHAR_SIZE];
    unsigned int colour;

    ScreenChar()
        : colour(0)
    {
        for (int ii = 0; ii < CHAR_SIZE; ii++)
        {
            contents[ii] = 0;
        }
    }

    ScreenChar(const unsigned char contents[CHAR_SIZE], unsigned int colour)
        : colour(colour)
    {
        for (int ii = 0; ii < CHAR_SIZE; ii++)
        {
            this->contents[ii] = contents[ii];
        }
    }

    void set(ScreenChar ch)
    {
        for (int ii = 0; ii < CHAR_SIZE; ii++)
        {
            contents[ii] = ch.contents[ii];
        }
        colour = ch.colour;
    }

    void set(ScreenChar ch, unsigned int overrideColour)
    {
        set(ch);
        colour = overrideColour;
    }
};
ScreenChar sc_egg(CHAR_EGG, EGG_COLOUR);
ScreenChar sc_empty(CHAR_EMPTY, EMPTY_COLOUR);
ScreenChar sc_snake_head(CHAR_SNAKE_HEAD, 0);
ScreenChar sc_player_mouth_open(CHAR_PLAYER_MOUTH_OPEN, PLAYER_COLOUR);
ScreenChar sc_player_mouth_closed(CHAR_PLAYER_MOUTH_CLOSED, PLAYER_COLOUR);
ScreenChar sc_snake_pit[SNAKE_PIT_ROWS][SNAKE_PIT_COLS];

typedef struct
{
    int x;
    int y;
} Point;

//
// Snake
//
// The snake
//
class Snake
{
public:
    enum Master { MASTER, NOT_MASTER };

    Point head;
    const Master master; // Can eat eggs
    const unsigned int colour;

    Snake(Point head, Master master, unsigned int colour) 
        : head(head), master(master), colour(colour)
    {
        myHead = sc_snake_head;
        myHead.colour = colour;
    }

    void placeOnScreen()
    {
        sc_snake_pit[head.y][head.x].set(myHead);
    }

private:
    ScreenChar myHead;

};

Snake snake[NUM_SNAKES] = 
{
    Snake({-1, -1}, Snake::MASTER,     0xaa0000), // red
    Snake({-1, -1}, Snake::NOT_MASTER, 0xaaaaaa), // white
    Snake({-1, -1}, Snake::NOT_MASTER, 0x00aaaa), // cyan
    Snake({-1, -1}, Snake::NOT_MASTER, 0xaa00aa), // purple
    Snake({-1, -1}, Snake::NOT_MASTER, 0x00aa00), // green
    Snake({-1, -1}, Snake::NOT_MASTER, 0x0000aa), // blue
    Snake({-1, -1}, Snake::NOT_MASTER, 0xaaaa00)  // yellow
};

class Player {
public:
    enum Animation { OPEN, CLOSED };

    Point pos;
    Animation animation;

    Player(Point pos, Animation animation) 
        : pos(pos), animation(animation)
    {}

    void placeOnScreen()
    {
        ScreenChar player_char;
        if (animation == Player::OPEN)
        {
            player_char = sc_player_mouth_open;
        }
        else
        {
            player_char = sc_player_mouth_closed;
        }
        sc_snake_pit[pos.y][pos.x].set(player_char);
    }
};
Player player({-1, -1}, Player::OPEN);

void Game::run()
{
    // (Re-)initialise the player and snakes
    init_player();
    init_snakes();

    // (Re-)initialise the snake pit
    init_snake_pit();

    // Place the player and snakes on the screen
    player.placeOnScreen();
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        snake[ii].placeOnScreen();
    }

    // Draw the snake pit
    render_snake_pit();

    // Round robin, with the player and each snake taking turns to move
}

void Game::init_player()
{
    player.pos = {19, 17};
    player.animation = Player::OPEN;
}

void Game::init_snakes()
{
    // Just update the positions - master and colour are correct and don't change
    int x = 1;
    int y = 2;
    
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        snake[ii].head = {x, y};

        x += 6;
        if (x > (SNAKE_PIT_COLS-3))
        {
            x = 1;
            y += 15;
        }
    }
}

void Game::init_snake_pit()
{
    // Fill the pit with eggs
    ScreenChar *cell = &sc_snake_pit[0][0];
    for (int ii = 0; ii < (SNAKE_PIT_ROWS * SNAKE_PIT_COLS); ii++)
    {
        cell->set(sc_egg);
        cell++;
    }

    // Clear 2x3 holes for each snake
    ScreenChar empty;
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        int x = snake[ii].head.x;
        int y = snake[ii].head.y;
        for (int xx = x; xx < x+2; xx++)
        {
            for (int yy = y; yy < y+3; yy++)
            {
                if ((xx < SNAKE_PIT_COLS) && (yy < SNAKE_PIT_ROWS))
                {
                    sc_snake_pit[yy][xx].set(sc_empty);
                }
            }
        }
    }
}

void Game::render_snake_pit()
{
    for (int row = 0; row < SNAKE_PIT_ROWS; row++)
    {
        for (int col = 0; col < SNAKE_PIT_COLS; col++)
        {
            // draw_char(snake_pit[row][col].contents, snake_pit[row][col].colour, 0, col, row);
        }
    }
}

#if 0
void *memcpy(void *dest, const void *src, int n)
{
    char *dest_char = (char *)dest;
    char *src_char = (char *)src;    
    while (n--)
    {
        *dest_char++ = *src_char++;
    }
    return dest;
}

void *memset(void *s, int c, unsigned int n)
{
    char *s_char = (char *)s;
    while (n--)
    {
        *s_char++ = c;
    }
    return s;
}

void clear_pos(unsigned char x, unsigned char y)
{
    ScreenChar ch;
    get_screen_char(&ch, CHAR_EMPTY, EMPTY_COLOUR);
    setScreenCell(&snake_pit[y][x], &ch);
}

void move_player(char dx, char dy)
{
    clear_pos(player.pos.x, player.pos.y);
    player.pos.x += dx;
    player.pos.y += dy;
    draw_player();
}
#endif



