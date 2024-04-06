#define GAME_CPP
#include "snakepit.h"
LOGMODULE("snakepit-game");

Game::Game(CDeviceNameService *deviceNameService,
           CScreenDevice *screen,
           CTimer *timer,
           CUSBHCIDevice *usbDevice)
    : deviceNameService(deviceNameService), screen(screen), timer(timer), usbDevice(usbDevice), keyboard(nullptr), player(create_player()), snake(create_snakes())
{
    LOGDBG("Game created");
}

bool Game::init()
{
    get_keyboard(true);
    return true;
}

void Game::get_keyboard(bool waitTilFound)
{
    LOGNOTE("Finding keyboard ...");
    while (!get_keyboard() && waitTilFound)
    LOGNOTE("Keyboard found");
}

bool Game::get_keyboard()
{
    if (keyboard == nullptr)
    {
        boolean updated = usbDevice->UpdatePlugAndPlay();
        if (updated)
        {
            keyboard = (CUSBKeyboardDevice *)deviceNameService->GetDevice("ukbd1", FALSE);
            if (keyboard != nullptr)
            {
                keyboard->RegisterKeyPressedHandler(KeyPressedHandler);
                keyboard->RegisterKeyReleasedHandler(KeyReleasedHandler);
                keyboard->RegisterRemovedHandler(KeyboardRemovedHandler);
                keyboard->RegisterShutdownHandler(KeyboardShutdownHandler);
            }
        }
    }
    return (keyboard != nullptr);
}

void KeyPressedHandler(const char *string)
{
    last_key_pressed = string[0];
}

void KeyReleasedHandler(const char *string)
{
    char key = string[0];
    if (key == last_key_pressed)
    {  
        last_key_pressed = 0;
    }
}

void KeyboardRemovedHandler(CDevice *device, void *context)
{
    LOGWARN("Keyboard removed");
    run = false;
}

void KeyboardShutdownHandler()
{
    LOGWARN("Keyboard shutdown");
    run = false;
}

void Game::go()
{
    run = true;

GAME_START:
    reset_game();

    // Round robin, with the player and each snake taking turns to move
    LOGNOTE("Game started");
    while (run)
    {
        for (int ii = 0; ii < NUM_SNAKES+1; ii++)
        {
            timer->MsDelay(1000/moves_per_s/(NUM_SNAKES+1));
            if (ii < NUM_SNAKES)
            {
                snake[ii]->takeTurn();
            }
            else if (!playerEaten)
            {
                player->takeTurn();
            }

            // Re-render the snake-pit
            // Will only re-render the changed cells if there are fewer than MAX_CHANGE_CELLS
            render_score_bar();
            render_snake_pit();

            if (playerEaten and (last_key_pressed == 's'))
            {
                while (last_key_pressed)
                {
                    // Wait for s key to be released
                    timer->MsDelay(50);
                }
                LOGDBG("Game restarted");
                goto GAME_START;
            }

            if (last_key_pressed == 'e')
            {
                LOGNOTE("Remaining eggs: %d", num_eggs);
            }

            while (last_key_pressed == 'p')
            {
                LOGDBG("Game paused");
                timer->MsDelay(50);
            }

            if (last_key_pressed == 'r')
            {
                LOGNOTE("Reset requested");
                timer->MsDelay(1000);
                run = false;
                break;
            }
        }
    }

    LOGNOTE("Main game routine exited");
}

void Game::reset_game()
{
    // Reset game variables
    playerEaten = false;
    numCellsChanged = 0;
    num_eggs = 0;
    moves_per_s = DEFAULT_MOVES_PER_S;
    highScore = (score > highScore ? score : highScore);
    score = 0;

    // Reset player and snakes
    init_player();
    init_snakes();

    // Reset the snake pit
    init_snake_pit();

    // Place the player and snakes on the screen
    player->placeOnScreen();
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        snake[ii]->placeOnScreen();
    }

    // Draw the snake pit
    render_border();
    render_score_bar();
    render_snake_pit();
}

Player* Game::create_player()
{
    LOGDBG("Creating player");
    Player* player = new Player(Point{-1, -1});
    return player;
}

void Game::init_player()
{
    LOGDBG("Initialising player");
    assert(player != nullptr);
    player->pos = {19, 17};
}

Snake** Game::create_snakes()
{
    Snake** snake = new Snake*[NUM_SNAKES];

    LOGDBG("Creating snakes");

    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        LOGDBG("Create snake %d at %d/%d", ii);
        Snake::Master master = (ii == 0 ? Snake::MASTER : Snake::NOT_MASTER);
        snake[ii] = new Snake({-1, -1}, master, SNAKE_COLOURS[ii]);
    }

    return snake;
}

void Game::init_snakes()
{
    LOGDBG("Initialising snakes");

    Point pos = Point{1, 2};
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        LOGDBG("Initialise snake %d at %d/%d", ii, pos.x, pos.y);
        assert((pos.x >= 0) && (pos.x < SNAKE_PIT_COLS));
        assert((pos.y >= 0) && (pos.y < SNAKE_PIT_ROWS));
        snake[ii]->init(pos);

        pos.x += 6;
        if (pos.x > (SNAKE_PIT_COLS-3))
        {
            pos.x = 1;
            pos.y += 15;
        }
    }
}

void Game::init_snake_pit()
{
    LOGDBG("Initialising snake pit");
    memset(sc_snake_pit, 0, sizeof(sc_snake_pit));

    // Fill the pit with eggs
    LOGDBG("Fill snake pit with eggs");
    for (int ii = 0; ii < SNAKE_PIT_COLS; ii++)
    {
        for (int jj = 0; jj < SNAKE_PIT_ROWS; jj++)
        {
            changeCell(Point{ii, jj}, sc_egg);
        }
    }
    LOGDBG("Snake pit filled with %d eggs", num_eggs);

    // Clear 2x3 holes for each snake
    LOGDBG("Empty snake nests");
    ScreenChar empty;
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        LOGDBG("Processing snake %d: %p", ii, snake[ii]);
        int x = snake[ii]->head.x;
        int y = snake[ii]->head.y;
        LOGDBG("Empty snake nest %d at %d/%d", ii, x, y);
        for (int xx = x; xx < x+2; xx++)
        {
            for (int yy = y; yy < y+3; yy++)
            {
                if ((xx < SNAKE_PIT_COLS) && (yy < SNAKE_PIT_ROWS))
                {
                    changeCell(Point{xx, yy}, sc_empty);
                }
            }
        }
    }
    LOGDBG("Snake pit contains %d eggs", num_eggs);
}

void Game::render_border()
{
    assert((SCREEN_ROW_OFFSET-BORDER_WIDTH) >= 0);
    assert((SCREEN_COL_OFFSET-BORDER_WIDTH) >= 0);
    assert((SCREEN_ROW_OFFSET+BORDER_WIDTH+(SNAKE_PIT_ROWS+1)*CHAR_SIZE*ZOOM_Y) <= SCREEN_HEIGHT);
    assert((SCREEN_COL_OFFSET+BORDER_WIDTH+SNAKE_PIT_COLS*CHAR_SIZE*ZOOM_X) <= SCREEN_WIDTH);

    // Inefficient as we'll be drawing the area the score bar and snake pit will cover over
    for (int ii = (SCREEN_COL_OFFSET - BORDER_WIDTH);
         ii < (SCREEN_COL_OFFSET + BORDER_WIDTH + SNAKE_PIT_COLS*CHAR_SIZE*ZOOM_X);
         ii++)
    {
        for (int jj = (SCREEN_ROW_OFFSET - BORDER_WIDTH - (1*CHAR_SIZE*ZOOM_Y));
             jj < (SCREEN_ROW_OFFSET + BORDER_WIDTH + (SNAKE_PIT_ROWS+1)*CHAR_SIZE*ZOOM_Y);
             jj++)
        {
            screen->SetPixel(ii, jj, TSCREEN_BORDER_COLOUR);
        }
    }
}

void Game::build_score_bar()
{
    assert(SCORE_BAR_COLS >= (2*MAX_SCORE_DIGITS + 1));
    assert(MAX_SCORE_DIGITS > 1);

    unsigned int next_char = 0;

    unsigned int tens = 1;
    for (int ii = 0; ii < MAX_SCORE_DIGITS-1; ii++)
    {
        tens *= 10;
    }

    for (int loopTens = tens; loopTens >= 1; loopTens /= 10)
    {
        sc_score_bar[next_char++].setDigit(score/loopTens % 10, SCORE_COLOUR, true);
    }

    for (; next_char < (SCORE_BAR_COLS-MAX_SCORE_DIGITS); next_char++)
    {
        sc_score_bar[next_char].set(sc_empty, SCORE_COLOUR, true);
    }

    assert((next_char + MAX_SCORE_DIGITS) == SCORE_BAR_COLS);
    for (int loopTens = tens; loopTens >= 1; loopTens /= 10)
    {
        sc_score_bar[next_char++].setDigit(highScore/loopTens % 10, SCORE_COLOUR, true);
    }
}

void Game::render_score_bar()
{
    LOGDBG("Render score bar");
    build_score_bar();
    for (int ii = 0; ii < SCORE_BAR_COLS; ii++)
    {
        draw_char(sc_score_bar[ii].contents,
                  sc_score_bar[ii].colour,
                  0,
                  ii,
                  0,
                  SCORE_BAR_COL_OFFSET,
                  SCORE_BAR_ROW_OFFSET);
    }
}

void Game::render_snake_pit()
{
    LOGDBG("Render snake pit - change cells: %d", numCellsChanged);
    if (numCellsChanged < MAX_CHANGED_CELLS)
    {
        LOGDBG("Render only changed cells");
        render_cells(cellsChanged, numCellsChanged);
    }
    else
    {
        LOGDBG("Render entire snake pit");
        for (int row = 0; row < SNAKE_PIT_ROWS; row++)
        {
            for (int col = 0; col < SNAKE_PIT_COLS; col++)
            {
                draw_char(sc_snake_pit[row][col].contents, sc_snake_pit[row][col].colour, 0, col, row);
            }
        }
    }
    numCellsChanged = 0;
}

void Game::render_cell(Point cellPos)
{
    draw_char(sc_snake_pit[cellPos.y][cellPos.x].contents, sc_snake_pit[cellPos.y][cellPos.x].colour, 0, cellPos.x, cellPos.y);
}

void Game::render_cells(Point *cellPos, unsigned int num)
{
    for (unsigned int ii = 0; ii < num; ii++)
    {
        render_cell(cellPos[ii]);
    }
}

#include <circle/debug.h>

void Game::draw_char(const unsigned char *contents, 
                     unsigned int colour,
                     unsigned char attr,
                     int x,
                     int y,
                     unsigned int screen_col_offset,
                     unsigned int screen_row_offset)
{
    TScreenColor screen_colour;
    switch (colour)
    {
        case EMPTY_COLOUR:
            screen_colour = BLACK_COLOR;
            break;
        case 0x0000aa:
            screen_colour = BRIGHT_BLUE_COLOR;
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
            screen_colour = BRIGHT_YELLOW_COLOR;
            break;
        case PLAYER_COLOUR:
        case 0xaaaaaa:
            screen_colour = WHITE_COLOR;
            break;
        case EGG_COLOUR:
            screen_colour = RED_COLOR;
            break;
        default:
            screen_colour = NORMAL_COLOR;
            break;
    }

    for (int yy = 0; yy < CHAR_SIZE; yy++)
    {
        unsigned char mask = 0x80;
        for (int xx = 0; xx < CHAR_SIZE; xx++)
        {
            // Either update the "pixel" to the screen colour or black
            TScreenColor updateColour;
            if (*contents & mask)
            {
                updateColour = screen_colour;
            }
            else
            {
                updateColour = BLACK_COLOR;
            }
        
            // Actually update the pixel (or pixels if zooming)
            for (int jj = 0; jj < ZOOM_Y; jj++)
            {
                for (int ii = 0; ii < ZOOM_X; ii++)
                {
                    int x_coords;
                    int y_coords;
                    x_coords = screen_col_offset + x * CHAR_SIZE * ZOOM_X + (xx * ZOOM_X) + ii;
                    y_coords = screen_row_offset + y * CHAR_SIZE * ZOOM_Y + (yy * ZOOM_Y) + jj;
                    screen->SetPixel(x_coords, y_coords, updateColour);
                }
            }

            // Move to next pixel
            mask >>= 1;
        }
        contents++;
    }
}
