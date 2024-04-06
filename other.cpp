#define OTHER_CPP
#include "snakepit.h"
LOGMODULE("snakepit-other");

void changeCell(Point cellPos, ScreenChar ch)
{
    sc_snake_pit[cellPos.y][cellPos.x].set(ch);
    if (numCellsChanged < MAX_CHANGED_CELLS)
    {
        cellsChanged[numCellsChanged].x = cellPos.x;
        cellsChanged[numCellsChanged].y = cellPos.y;
    }
    numCellsChanged++;
}

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

void ScreenChar::setDigit(unsigned int digit, bool reverse)
{
    assert(digit < 10);
    set(CHAR_DIGITS[digit], reverse);
}

void ScreenChar::setDigit(unsigned int digit, unsigned int overrideColour, bool reverse)
{
    set(CHAR_DIGITS[digit], overrideColour, reverse);
}

void ScreenChar::set(ScreenChar ch, bool reverse)
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
        contents[ii] = (!reverse) ? ch.contents[ii] : ~ch.contents[ii];
    }
    colour = ch.colour;
}

void ScreenChar::set(ScreenChar ch, unsigned int overrideColour, bool reverse)
{
    set(ch, reverse);
    colour = overrideColour;
}

Snake::Snake(Point head, Master master, unsigned int colour) 
    : head(head), master(master), colour(colour)
{
    myHead = sc_snake_head_down;
    myHead.colour = colour;
    lastDirection = Snake::DOWN; // Snakes always start in top left of their hole, as if they were going down
}

void Snake::placeOnScreen()
{
    LOGDBG("Placing snake head at %d/%d", head.x, head.y);
    switch (lastDirection)
    {
        case UP:
            myHead.set(sc_snake_head_up, colour);
            break;
        case DOWN:
            myHead.set(sc_snake_head_down, colour);
            break;
        case LEFT:
            myHead.set(sc_snake_head_left, colour);
            break;
        case RIGHT:
            myHead.set(sc_snake_head_right, colour);
            break;
    }
    if (sc_snake_pit[head.y][head.x].colour == PLAYER_COLOUR)
    {
        LOGNOTE("Game over - player eaten");
        playerEaten = true;
    }
    changeCell(head, myHead);
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
    changeCell(current, sc_empty);

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
        //LOGNOTE("Moving red snake result: %d, options up: %d, down: %d, left: %d, right: %d", randomNum, probability[0], probability[1], probability[2], probability[3]);
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

Player::Player(Point pos) 
        : pos(pos), animation(Player::OPEN)
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
    LOGDBG("Placing player at %d/%d", pos.x, pos.y);
    changeCell(pos, player_char);
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
        LOGDBG("Player key pressed: %c", last_key_pressed);
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
                if (sc_snake_pit[next.y][next.x].colour == EGG_COLOUR)
                {
                    score += SCORE_EGG_INCREMENT;
                }
                changeCell(pos, sc_empty);
                pos = next;
                // Player is updated below - as even if they don't move we want to change animations
            }
        }

    }
    placeOnScreen();
}

