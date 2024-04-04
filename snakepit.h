#ifndef SNAKEPIT_H
#define SNAKEPIT_H

#include <circle/logger.h>

static const char FromSnakepit[] = "snakepit";

class Game
{
public:
    Game(CLogger logger);
    void run();

private:
    CLogger logger;
    void init_player();
    void init_snakes();
    void init_snake_pit();
    void render_snake_pit();
};

#endif // SNAKEPIT_H

