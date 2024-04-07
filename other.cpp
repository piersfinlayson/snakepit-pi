#define OTHER_CPP
#include "snakepit.h"
LOGMODULE("snakepit-other");

void changeCell(Point cellPos, ScreenChar ch)
{
    assert(cellPos.x >= 0 && cellPos.y >= 0);

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
    if (colour == EGG_COLOUR)
    {
        num_eggs--;
        if (num_eggs == 0)
        {
            LOGNOTE("All eggs eaten");
        }
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
    : head(head), master(master), colour(colour), animation(OPEN), bodyLen(0), bodyHead(0), nextHead(Point{-1, -1}), lastDirection(Snake::DOWN), nextDirection(Snake::DOWN)
{
    myHead = sc_snake_head_down;
    myHead.colour = colour;
    bodyPart[0].bodyChar.set(myHead, colour);
    bodyPart[0].pos = head;
    bodyHead = 0;
    bodyLen = 1;
}

void Snake::init(Point pos)
{
    head = pos;
    nextHead = pos;
    lastDirection = DOWN;
    nextDirection = DOWN;
    bodyHead = 0;
    bodyLen = 1;
    bodyPart[bodyHead].pos = pos;
}

void Snake::updateBody()
{
    assert(bodyLen > 0);
    int oldBodyHead = bodyHead;
    int bodyHeadMinus2 = bodyHead - 1;

    // To go back 2 from the nextHead, we need a body length of at least 2 already
    if (bodyLen > 1)
    {
        if (bodyHeadMinus2 < 0)
        {
            bodyHeadMinus2 = SNAKE_BODY_LEN-1;
        }
    }
    else
    {
        // Can't use 2 back from nextHead
        bodyHeadMinus2 = -1;
    }

    bodyHead++;
    if (bodyHead >= SNAKE_BODY_LEN)
    {
        bodyHead = 0;
    }

    if (bodyLen == SNAKE_BODY_LEN)
    {
        // Clear out old tail - bodyHead currently points to the tail
        // But only if there's nothing on top of it.
        Point *tailPos = &(bodyPart[bodyHead].pos);
        bool clear = true;
        for (int ii = 0; ii < SNAKE_BODY_LEN; ii++)
        {
            if ((*tailPos == bodyPart[ii].pos) && (ii != bodyHead))
            {
                clear = false;
                break;
            }
        }
        if (clear)
        {
            changeCell(*tailPos, sc_empty);
        }
    }

    // Figure out if oldBodyhead is left, right, up or down of nextHead
    Direction oldHead;
    if (bodyPart[oldBodyHead].pos.x < bodyPart[bodyHead].pos.x)
    {
        oldHead = LEFT;
    }
    else if (bodyPart[oldBodyHead].pos.y < bodyPart[bodyHead].pos.y)
    {
        oldHead = UP;
    }
    else if (bodyPart[oldBodyHead].pos.y > bodyPart[bodyHead].pos.y)
    {
        oldHead = DOWN;
    }
    else if (bodyPart[oldBodyHead].pos.x > bodyPart[bodyHead].pos.x)
    {
        oldHead = RIGHT;
    }
    {
        assert(false);
    }

    // And figure out where the body was before then
    Direction headMinus2;
    if (bodyHeadMinus2 >= 0)
    {
        if (bodyPart[bodyHeadMinus2].pos.x < bodyPart[oldBodyHead].pos.x)
        {
            headMinus2 = LEFT;
        }
        else if (bodyPart[bodyHeadMinus2].pos.y < bodyPart[oldBodyHead].pos.y)
        {
            headMinus2 = UP;
        }
        else if (bodyPart[bodyHeadMinus2].pos.y > bodyPart[oldBodyHead].pos.y)
        {
            headMinus2 = DOWN;
        }
        else if (bodyPart[bodyHeadMinus2].pos.x > bodyPart[oldBodyHead].pos.x)
        {
            headMinus2 = RIGHT;
        }
        {
            assert(false);
        }
    }

    // Now we have what we need to figure out body part before the head
    const unsigned char *newBodyPart;
    switch (oldHead)
    {
        case LEFT:
            newBodyPart = CHAR_SNAKE_BODY_HORIZONTAL;
            if (bodyHeadMinus2 >= 0)
            {
                switch (headMinus2)
                {
                    case LEFT:
                        break;
                    case RIGHT:
                        newBodyPart = CHAR_SNAKE_BODY_DBL_RIGHT;
                        break;
                    case UP:
                        newBodyPart = CHAR_SNAKE_BODY_R_U;
                        break;
                    case DOWN:
                        newBodyPart = CHAR_SNAKE_BODY_R_D;
                        break;
                    default:
                        assert(false);
                        break;
                }
            }
            break;
        case RIGHT:
            newBodyPart = CHAR_SNAKE_BODY_HORIZONTAL;
            if (bodyHeadMinus2 >= 0)
            {
                switch (headMinus2)
                {
                    case LEFT:
                        newBodyPart = CHAR_SNAKE_BODY_DBL_LEFT;
                        break;
                    case RIGHT:
                        break;
                    case UP:
                        newBodyPart = CHAR_SNAKE_BODY_L_U;
                        break;
                    case DOWN:
                        newBodyPart = CHAR_SNAKE_BODY_L_D;
                        break;
                    default:
                        assert(false);
                        break;
                }
            }
            break;
        case UP:
            newBodyPart = CHAR_SNAKE_BODY_VERTICAL;
            if (bodyHeadMinus2 >= 0)
            {
                switch (headMinus2)
                {
                    case LEFT:
                        newBodyPart = CHAR_SNAKE_BODY_R_U;
                        break;
                    case RIGHT:
                        newBodyPart = CHAR_SNAKE_BODY_L_U;
                        break;
                    case UP:
                        break;
                    case DOWN:
                        newBodyPart = CHAR_SNAKE_BODY_DBL_DOWN;
                        break;
                    default:
                        assert(false);
                        break;
                }
            }
            break;
        case DOWN:
            newBodyPart = CHAR_SNAKE_BODY_VERTICAL;
            if (bodyHeadMinus2 >= 0)
            {
                switch (headMinus2)
                {
                    case LEFT:
                        newBodyPart = CHAR_SNAKE_BODY_R_D;
                        break;
                    case RIGHT:
                        newBodyPart = CHAR_SNAKE_BODY_L_D;
                        break;
                    case UP:
                        newBodyPart = CHAR_SNAKE_BODY_DBL_UP;
                        break;
                    case DOWN:
                        break;
                    default:
                        assert(false);
                        break;
                }
            }
            break;
        default:
            assert(false);
            break;
    }

    // Change the old head to be the corret body part105
    ScreenChar scNewBodyPart = {newBodyPart, colour};
    bodyPart[oldBodyHead].bodyChar.set(scNewBodyPart);
    changeCell(bodyPart[oldBodyHead].pos, bodyPart[oldBodyHead].bodyChar);

    // Figure out next head
    LOGDBG("Placing snake head at %d/%d", nextHead.x, nextHead.y);
    ScreenChar *ch = &sc_snake_head_down;
    switch (nextDirection)
    {
        case UP:
            ch = &sc_snake_head_up;
            break;
        case DOWN:
            ch = &sc_snake_head_down;
            break;
        case LEFT:
            ch = &sc_snake_head_left;
            break;
        case RIGHT:
            ch = &sc_snake_head_right;
            break;
        default:
            assert(false);
            break;
    }
    myHead.set(*ch, colour);

    // Set the new head position
    bodyPart[bodyHead].bodyChar.set(myHead, colour);
    bodyPart[bodyHead].pos = nextHead;
    if (bodyLen < SNAKE_BODY_LEN)
    {
        bodyLen++;
    }

    // Now we've made the move, update our variables
    lastDirection = nextDirection;
    head = nextHead;

    changeCell(head, myHead);
}

void Snake::placeOnScreen()
{
    makeMove();
}

void Snake::takeTurn()
{
    generateNextDirection();
    generateNextPosition();
    makeMove();
}

void Snake::makeMove()
{
    assert(nextHead.x >= 0 && nextHead.y >= 0);

    if (sc_snake_pit[nextHead.y][nextHead.x].colour == PLAYER_COLOUR)
    {
        LOGNOTE("Game over - player eaten");
        playerEaten = true;
    }

    updateBody();
}

void Snake::generateNextPosition()
{
    // Set the next position based on the direction
    nextHead = head;
    switch (nextDirection)
    {
        case UP:
            nextHead.y--;
            break;
        case DOWN:
            nextHead.y++;
            break;
        case LEFT:
            nextHead.x--;
            break;
        case RIGHT:
            nextHead.x++;
            break;
        default:
            assert(false);
            break;
    }
}


void Snake::generateNextDirection()
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
        nextDirection = lastResortDirection;
        return;
    }

    // If we can only go one way, go that way
    if (validDirections == 1)
    {
        for (int ii = 0; ii < 4; ii++)
        {
            if (allowed[ii])
            {
                nextDirection = direction[ii];
                return;
            }
        }
        nextDirection = lastResortDirection; // Belt and braces
        return;
    }

    // More than one valid direction, weight them
    unsigned int percent = 0;
    int percentPer;
    if (preferredIndex >= 0)
    {
        assert(preferredIndex < 4);
        // We have a preferred direction, overweight that
        probability[preferredIndex] = 80;
        percent = probability[preferredIndex];
        validDirections--;
        percentPer = (100-probability[preferredIndex])/validDirections;
    }
    else
    {
        percentPer = 100/validDirections;
    }

    // Set probabilities for non-preferred directions, with equal weighting
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
    LOGDBG("Moving snake result: %d, options up: %d, down: %d, left: %d, right: %d", randomNum, probability[0], probability[1], probability[2], probability[3]);

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
    nextDirection = lowestProbDirection;
    return;
}

Player::Player(Point pos) 
        : pos(pos), animation(OPEN)
{}

void Player::placeOnScreen()
{
    ScreenChar player_char;
    if (animation == OPEN)
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
    if (animation == OPEN)
    {
        animation = CLOSED;
    }
    else
    {
        animation = OPEN;
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

