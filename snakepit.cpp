#include "snakepit.h"

#define CHAR_SIZE 8

#define SCREEN_ROW_OFFSET 100
#define SCREEN_COL_OFFSET 1000

#define SNAKE_PIT_ROWS 22
#define SNAKE_PIT_COLS 22
#define NUM_SNAKES 7

// How many times bigger to make the game image
#define ZOOM 4

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
#define EGG_COLOUR 0xaa0000
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

Game::Game(CLogger logger, CScreenDevice screen, CDeviceNameService deviceNameService, CUSBHCIDevice usbhci)
    : logger(logger), screen(screen), deviceNameService(deviceNameService), usbhci(usbhci)
{
    keyboard = nullptr;
}

void Game::init()
{
    get_keyboard();
}

void Game::get_keyboard()
{
    if (keyboard == nullptr)
    {
        boolean updated = usbhci.UpdatePlugAndPlay();
        if (updated)
        {
            keyboard = (CUSBKeyboardDevice *)deviceNameService.GetDevice("ukbd1", FALSE);
            if (keyboard != nullptr)
            {
#if 0
                keyboard->RegisterRemovedHandler(KeyboardRemovedHandler);
                keyboard->RegisterShutdownHandler(ShutdownHandler);
                keyboard->RegisterKeyPressedHandler(KeyPressedHandler);
#endif
            }
        }

    }
}

void Game::run()
{
    logger.Write(FromSnakepit, LogNotice, "Game::run() called");
    logger.Write(FromSnakepit, LogNotice, "Colour depth: %d", DEPTH);

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

    logger.Write(FromSnakepit, LogNotice, "Game::run() exited");
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
            draw_char(sc_snake_pit[row][col].contents, sc_snake_pit[row][col].colour, 0, col, row);
        }
    }
}

void Game::draw_char(const unsigned char *contents, unsigned int colour, unsigned char attr, int x, int y)
{
    TScreenColor screen_colour;
    switch (colour)
    {
        case 0x000000:
            screen_colour = BLACK_COLOR;
            break;
        case 0x0000aa:
            screen_colour = BLUE_COLOR;
            break;
        case 0x00aa00:
            screen_colour = GREEN_COLOR;
            break;
        case 0x00aaaa:
            screen_colour = CYAN_COLOR;
            break;
        case 0xaa0000:
            screen_colour = RED_COLOR;
            break;
        case 0xaa00aa:
            screen_colour = MAGENTA_COLOR;
            break;
        case 0xaaaa00:
            screen_colour = YELLOW_COLOR;
            break;
        case 0xaaaaaa:
            screen_colour = WHITE_COLOR;
            break;
        default:
            screen_colour = NORMAL_COLOR;
            break;
    }

    for (int yy = 0; yy < CHAR_SIZE; yy++)
    {
        unsigned char mask = 1;
        for (int xx = 0; xx < CHAR_SIZE; xx++)
        {
            if (*contents & mask)
            {
                for (int ii = 0; ii < ZOOM; ii++)
                {
                    int x_coords;
                    int y_coords;
                    x_coords = SCREEN_COL_OFFSET;
                    x_coords += x * CHAR_SIZE * ZOOM;
                    x_coords += (xx * ZOOM) + ii;
                    y_coords = SCREEN_ROW_OFFSET;
                    y_coords += y * CHAR_SIZE * ZOOM;
                    y_coords += (yy * ZOOM) + ii;
                    screen.SetPixel(x_coords, y_coords, screen_colour);
                }
            }
            mask <<= 1;
        }
        contents++;
    }
}

#if 0

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



