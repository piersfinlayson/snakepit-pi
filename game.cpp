#define GAME_CPP
#include "snakepit.h"
LOGMODULE("snakepit-game");

Game::Game(CDeviceNameService *deviceNameService,
           CScreenDevice *screen,
           CTimer *timer,
           CUSBHCIDevice *usbDevice)
    : deviceNameService(deviceNameService), screen(screen), timer(timer), usbDevice(usbDevice), keyboard(nullptr), player(init_player()), snake(init_snakes())
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
    bool found = false;
    if (keyboard != nullptr)
    {
        found = true;
    }
    else
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
    return found;
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
GAME_START:

    // (Re-)initialise the snake pit
    init_snake_pit();

    // Place the player and snakes on the screen
    player->placeOnScreen();
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        snake[ii]->placeOnScreen();
    }

    // Draw the snake pit
    render_snake_pit();

    // Round robin, with the player and each snake taking turns to move
    run = true;
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
            if (num_eggs <= 0)
            {
                LOGDBG("All eggs eaten");
            }

            // Re-render the snake-pit
            // Will only re-render the changed cells if there are fewer than MAX_CHANGE_CELLS
            render_snake_pit();

            if (playerEaten and (last_key_pressed == 's'))
            {
                while (last_key_pressed)
                {
                    // Wait for s key to be released
                    timer->MsDelay(50);
                }
                LOGDBG("Game restarted");
                reset_game();
                goto GAME_START;
            }
        }
    }

    LOGDBG("Game::run() exited");
}

void Game::reset_game()
{
    playerEaten = false;
    run = false;
    numCellsChanged = 0;
    num_eggs = 0;
    reset_player();
    reset_snakes();
}

Player* Game::init_player()
{
    Point pos = {19, 17};
    LOGDBG("Initialising player at %d/%d", pos.x, pos.y);
    Player* player = new Player(pos);
    return player;
}

Snake** Game::init_snakes()
{
    Snake** snake = new Snake*[NUM_SNAKES];

    LOGDBG("Initialising snakes");

    Point pos = Point{1, 2};
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        LOGDBG("Initialise snake %d at %d/%d", ii, pos.x, pos.y);
        Snake::Master master = (ii == 0 ? Snake::MASTER : Snake::NOT_MASTER);
        snake[ii] = new Snake(pos, master, SNAKE_COLOURS[ii]);
        LOGDBG("Snake %d: %p at %d/%d", ii, snake[ii], snake[ii]->head.x, snake[ii]->head.y);

        pos.x += 6;
        if (pos.x > (SNAKE_PIT_COLS-3))
        {
            pos.x = 1;
            pos.y += 15;
        }
    }

    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        LOGDBG("Snake %d: %p at %d/%d", ii, snake[ii], snake[ii]->head.x,  snake[ii]->head.y);
    }
    return snake;
}

void Game::reset_player()
{
    LOGDBG("Resetting player");
    delete player;
    player = init_player();
}

void Game::reset_snakes()
{
    LOGDBG("Resetting snakes");
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        delete snake[ii];
    }
    snake = init_snakes();
}

void Game::init_snake_pit()
{
    LOGDBG("Initialising snake pit");

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
        case 0xbbbbbb: // PLAYER_COLOUR
        case 0xaaaaaa:
            screen_colour = WHITE_COLOR;
            break;
        case 0xaa5555:
            screen_colour = BRIGHT_RED_COLOR;
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
                    x_coords = SCREEN_COL_OFFSET + x * CHAR_SIZE * ZOOM_X + (xx * ZOOM_X) + ii;
                    y_coords = SCREEN_ROW_OFFSET + y * CHAR_SIZE * ZOOM_Y + (yy * ZOOM_Y) + jj;
                    screen->SetPixel(x_coords, y_coords, updateColour);
                }
            }

            // Move to next pixel
            mask <<= 1;
        }
        contents++;
    }
}
