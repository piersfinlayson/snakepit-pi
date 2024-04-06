#include "snakepit.h"

static int num_eggs;
static unsigned int moves_per_s = 5;
volatile static char last_key_pressed = 0;
static bool run = false;
static bool playerEaten = false;

static Point cellsChanged[SNAKE_PIT_ROWS * SNAKE_PIT_COLS];
static unsigned int numCellsChanged = 0;

LOGMODULE("snakepit");
static const char FromSnakepit[] = "snakepit";

ScreenChar sc_egg(CHAR_EGG, EGG_COLOUR);
ScreenChar sc_empty(CHAR_EMPTY, EMPTY_COLOUR);
ScreenChar sc_snake_head(CHAR_SNAKE_HEAD, 0);
ScreenChar sc_snake_head_down(CHAR_SNAKE_HEAD_DOWN, 0);
ScreenChar sc_player_mouth_open(CHAR_PLAYER_MOUTH_OPEN, PLAYER_COLOUR);
ScreenChar sc_player_mouth_closed(CHAR_PLAYER_MOUTH_CLOSED, PLAYER_COLOUR);
ScreenChar sc_snake_pit[SNAKE_PIT_ROWS][SNAKE_PIT_COLS];

ScreenChar::ScreenChar()
    : colour(0)
{
    for (int ii = 0; ii < CHAR_SIZE; ii++)
    {
        contents[ii] = 0;
    }
}

ScreenChar::ScreenChar(const unsigned char contents[CHAR_SIZE], unsigned int colour)
    : colour(colour)
{
    for (int ii = 0; ii < CHAR_SIZE; ii++)
    {
        this->contents[ii] = contents[ii];
    }
}

void ScreenChar::set(ScreenChar ch)
{
    if (ch.colour == EGG_COLOUR)
    {
        num_eggs++;
    }
    else if ((ch.colour == EMPTY_COLOUR) && (colour == EGG_COLOUR))
    {
        num_eggs--;
    }
    assert(num_eggs >= 0);

    for (int ii = 0; ii < CHAR_SIZE; ii++)
    {
        contents[ii] = ch.contents[ii];
    }
    colour = ch.colour;
}

void ScreenChar::set(ScreenChar ch, unsigned int overrideColour)
{
    set(ch);
    colour = overrideColour;
}

Snake::Snake(Point head, Master master, unsigned int colour, CLogger *logger) 
    : head(head), master(master), colour(colour), logger(logger)
{
    myHead = sc_snake_head;
    myHead.colour = colour;
    lastDirection = Snake::DOWN; // Snakes always start in top left of their hole, as if they were going down
}

void Snake::placeOnScreen()
{
    //logger->Write(FromSnakepit, LogDebug, "Placing snake head at %d/%d", head.x, head.y);
    switch (lastDirection)
    {
        case UP:
            myHead.set(sc_snake_head, colour);
            break;
        case DOWN:
            myHead.set(sc_snake_head_down, colour);
            break;
        case LEFT:
            myHead.set(sc_snake_head, colour);
            break;
        case RIGHT:
            myHead.set(sc_snake_head, colour);
            break;
    }
    if (sc_snake_pit[head.y][head.x].colour == PLAYER_COLOUR)
    {
        logger->Write(FromSnakepit, LogDebug, "Snake just ate player");
        playerEaten = true;
    }
    sc_snake_pit[head.y][head.x].set(myHead);
    cellsChanged[numCellsChanged] = head;
    numCellsChanged++;
}

void Snake::takeTurn()
{
    makeMove(generateMove());
}

void Snake::makeMove(Direction direction)
{
    Point current = head;
    Point next = head;

    // Move the head
    switch (direction)
    {
        case UP:
            next.y--;
            break;
        case DOWN:
            next.y++;
            break;
        case LEFT:
            next.x--;
            break;
        case RIGHT:
            next.x++;
            break;
    }

    // Clear the current position
    sc_snake_pit[current.y][current.x].set(sc_empty);
    cellsChanged[numCellsChanged] = current;
    numCellsChanged++;

    // Save the direction
    lastDirection = direction;

    // Place the head on the screen
    head = next;
    placeOnScreen();

}

Snake::Direction Snake::generateMove()
{
    // Snakes prefer to move in the direction they were last going
    // However, they will, with a certain probability, choose a different random direction
    // However, they can't move over another snake, and non-master snakes can't move over eggs
    // Snakes will only go back the way they came if they have no other choice
    
    // Set the preferred direction based on the last direction
    Direction preferredDirection = lastDirection;
    Direction lastResortDirection;
    switch (preferredDirection)
    {
        case UP:
            lastResortDirection = DOWN;
            break;
        case DOWN:
            lastResortDirection = UP;
            break;
        case LEFT:
            lastResortDirection = RIGHT;
            break;
        case RIGHT:
            lastResortDirection = LEFT;
            break;
        default:
            lastResortDirection = DOWN;
            assert(false);
            break;
    }

    // Figure out what directions we can go in
    bool allowed[4] = {true, true, true, true};
    bool preferred[4] = {false, false, false, false};
    Direction direction[4] = {UP, DOWN, LEFT, RIGHT};
    unsigned int probability[4] = {0, 0, 0, 0};
    int preferredIndex = -1;
    int validDirections = 0;
    for (int ii = 0; ii < 4; ii++)
    {
        if (preferredDirection == direction[ii])
        {
            preferred[ii] = true;
        }

        Point directionPos;
        unsigned int directionColour;
        switch (direction[ii])
        {
            case UP:
                if (head.y <= 0) 
                {
                    allowed[ii] = false;
                }
                else
                {
                    directionPos = {head.x, head.y-1};
                }
                break;

            case DOWN:
                if (head.y >= SNAKE_PIT_ROWS-1)
                {
                    allowed[ii] = false;
                }
                else
                {
                    directionPos = {head.x, head.y+1};
                }
                break;

            case LEFT:
                if (head.x <= 0)
                {
                    allowed[ii] = false;
                }
                else
                {
                    directionPos = {head.x-1, head.y};
                }
                break;

            case RIGHT:
                if (head.x >= SNAKE_PIT_COLS-1)
                {
                    allowed[ii] = false;
                }
                else
                {
                    directionPos = {head.x+1, head.y};
                }
                break;
            
            default:
                directionPos = {0, 0};
                assert(false);
                break;
        }

        if (allowed[ii])
        {
            // We use the colour of that direction to decide whether it contains something we can't move over
            directionColour = sc_snake_pit[directionPos.y][directionPos.x].colour;

            // Non-master snakes can only go into empty cells, or back over themselves
            if ((directionColour != EMPTY_COLOUR) && (directionColour != colour) && (directionColour != PLAYER_COLOUR))
            {
                allowed[ii] = false;
            }

            // Master snakes can go over eggs
            if ((master == MASTER) && (directionColour == EGG_COLOUR))
            {
                allowed[ii] = true;
            }
        }

        //  Only go back the way we came if we have no other choice
        if (lastResortDirection == direction[ii])
        {
            allowed[ii] = false;
            preferred[ii] = false;
        }

        // If we can't go a direction, clearly we can't prefer it
        if (!allowed[ii])
        {
            preferred[ii] = false;
        }
        else
        {
            validDirections++;
        }

        if (preferred[ii])
        {
            assert(preferredIndex < 0);
            preferredIndex = ii;
        }
    }

    // If we can't move anywhere, go back the way we came
    if (!validDirections)
    {
        return lastResortDirection;
    }

    // If we can only go one way, go that way
    if (validDirections == 1)
    {
        for (int ii = 0; ii < 4; ii++)
        {
            if (allowed[ii])
            {
                return direction[ii];
            }
        }
        return lastResortDirection; // Belt and braces
    }

    // More than one valid direction, weight them
    unsigned int percent = 0;
    int percentPer;
    if (preferredIndex >= 0)
    {
        // We have a preferred direction, overweight that
        probability[preferredIndex] = 80;
        percent = probability[preferredIndex];
        validDirections--;
    }

    // Set probabilities for non-preferred directions, with equal weighting
    percentPer = (100-probability[preferredIndex])/validDirections;
    for (int ii = 0; ii < 4; ii++)
    {
        if (allowed[ii] && (ii != preferredIndex))
        {
            percent += percentPer;
            probability[ii] = percent;
            validDirections--;
            assert(validDirections >= 0);
        }
    }
    assert(validDirections == 0);

    // Generate a random number between 0 and 99 and turn into 1-100.
    unsigned int randomNum = CBcmRandomNumberGenerator().GetNumber() % percent + 1;
    //if (master == MASTER)
    //{
        //logger->Write(FromSnakepit, LogNotice, "Moving red snake result: %d, options up: %d, down: %d, left: %d, right: %d", randomNum, probability[0], probability[1], probability[2], probability[3]);
    //}

    Snake::Direction lowestProbDirection = lastDirection; // Initialize to a default value
    unsigned int lowestProb = 101; // Initialize to a value greater than any probability
    for (int ii = 0; ii < 4; ii++)
    {
        if (randomNum <= probability[ii] && probability[ii] < lowestProb)
        {
            lowestProb = probability[ii];
            lowestProbDirection = direction[ii];
        }
    }
    return lowestProbDirection;
}

Player::Player(Point pos, CLogger *logger) 
        : pos(pos), animation(Player::OPEN), logger(logger)
{}

void Player::placeOnScreen()
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
    //logger->Write(FromSnakepit, LogDebug, "Placing player at %d/%d", pos.x, pos.y);
    sc_snake_pit[pos.y][pos.x].set(player_char);
    cellsChanged[numCellsChanged] = pos;
    numCellsChanged++;

}

void Player::takeTurn()
{
    if (animation == Player::OPEN)
    {
        animation = Player::CLOSED;
    }
    else
    {
        animation = Player::OPEN;
    }

    if (last_key_pressed != 0)
    {
        //logger->Write(FromSnakepit, LogDebug, "Player key pressed: %c", last_key_pressed);
    }
    Point current = pos;
    Point next = pos;
    switch (last_key_pressed)
    {
        case 'z':
            // left
            next.x--;
            break;
        case 'x':
            // right
            next.x++;
            break;
        case ';':
            // up
            next.y--;
            break;
        case '.':
            // down
            next.y++;
            break;
    }

    // Check if user an move in the desired direction
    if (next != current)
    {
        if (next.x >= 0 && next.x < SNAKE_PIT_COLS && next.y >= 0 && next.y < SNAKE_PIT_ROWS)
        {
            if ((sc_snake_pit[next.y][next.x].colour == EGG_COLOUR) ||
                (sc_snake_pit[next.y][next.x].colour == EMPTY_COLOUR))
            {
                sc_snake_pit[pos.y][pos.x].set(sc_empty);
                cellsChanged[numCellsChanged] = pos;
                numCellsChanged++;

                pos = next;
                // Player is updated below - as even if they don't move we want to change animations
            }
        }

    }
    placeOnScreen();
}

Game::Game(CDeviceNameService *deviceNameService,
           CScreenDevice *screen,
           CTimer *timer,
           CLogger *logger,
           CUSBHCIDevice *usbDevice)
    : deviceNameService(deviceNameService), screen(screen), timer(timer), logger(logger), usbDevice(usbDevice), keyboard(nullptr), player(init_player()), snake(init_snakes())
{
    LOGNOTE("Game created");
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
    logger->Write(FromSnakepit, LogDebug, "Game started");

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
                logger->Write(FromSnakepit, LogDebug, "All eggs eaten");
            }
            render_cells(cellsChanged, numCellsChanged);
            // Not deleting cellsChanged - may be memory leak
            numCellsChanged = 0;
            if (playerEaten and (last_key_pressed == 's'))
            {
                while (last_key_pressed)
                {
                    // Wait for s key to be released
                    timer->MsDelay(50);
                }
                logger->Write(FromSnakepit, LogDebug, "Game restarted");
                reset_game();
                goto GAME_START;
            }
        }
    }

    logger->Write(FromSnakepit, LogDebug, "Game::run() exited");
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
    //logger->Write(FromSnakepit, LogDebug, "Initialising player at %d/%d", pos.x, pos.y);
    Player* player = new Player(pos, logger);
    return player;
}

Snake** Game::init_snakes()
{
    Snake** snake = new Snake*[NUM_SNAKES];

    //logger->Write(FromSnakepit, LogDebug, "Initialising snakes");

    Point pos = Point{1, 2};
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        //logger->Write(FromSnakepit, LogDebug, "Initialise snake %d at %d/%d", ii, pos.x, pos.y);
        Snake::Master master = (ii == 0 ? Snake::MASTER : Snake::NOT_MASTER);
        snake[ii] = new Snake(pos, master, SNAKE_COLOURS[ii], logger);
        //logger->Write(FromSnakepit, LogDebug, "Snake %d: %p at %d/%d", ii, snake[ii], snake[ii]->head.x, snake[ii]->head.y);

        pos.x += 6;
        if (pos.x > (SNAKE_PIT_COLS-3))
        {
            pos.x = 1;
            pos.y += 15;
        }
    }

    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        //logger->Write(FromSnakepit, LogDebug, "Snake %d: %p at %d/%d", ii, snake[ii], snake[ii]->head.x,  snake[ii]->head.y);
    }
    return snake;
}

void Game::reset_player()
{
    logger->Write(FromSnakepit, LogDebug, "Resetting player");
    delete player;
    player = init_player();
}

void Game::reset_snakes()
{
    logger->Write(FromSnakepit, LogDebug, "Resetting snakes");
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        delete snake[ii];
    }
    snake = init_snakes();
}

void Game::init_snake_pit()
{
    //logger->Write(FromSnakepit, LogDebug, "Initialising snake pit");

    // Fill the pit with eggs
    //logger->Write(FromSnakepit, LogDebug, "Fill snake pit with eggs");
    ScreenChar *cell = &sc_snake_pit[0][0];
    for (int ii = 0; ii < (SNAKE_PIT_ROWS * SNAKE_PIT_COLS); ii++)
    {
        cell->set(sc_egg);
        cell++;
    }
    //logger->Write(FromSnakepit, LogDebug, "Snake pit filled with %d eggs", num_eggs);

    // Clear 2x3 holes for each snake
    //logger->Write(FromSnakepit, LogDebug, "Empty snake nests");
    ScreenChar empty;
    for (int ii = 0; ii < NUM_SNAKES; ii++)
    {
        //logger->Write(FromSnakepit, LogDebug, "Processing snake %d: %p", ii, snake[ii]);
        int x = snake[ii]->head.x;
        int y = snake[ii]->head.y;
        //logger->Write(FromSnakepit, LogDebug, "Empty snake nest %d at %d/%d", ii, x, y);
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
    logger->Write(FromSnakepit, LogDebug, "Snake pit contains %d eggs", num_eggs);
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



