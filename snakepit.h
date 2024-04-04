#ifndef SNAKEPIT_H
#define SNAKEPIT_H

#include <circle/logger.h>
#include <circle/screen.h>
#include <circle/koptions.h>

static const char FromSnakepit[] = "snakepit";

class Game
{
public:
    CLogger logger;
    Game(CLogger logger, CKernelOptions kernelOptions);
    void run();

private:
    CKernelOptions kernelOptions;
    CScreenDevice screen;
    void init_player();
    void init_snakes();
    void init_snake_pit();
    void render_snake_pit();
    void draw_char(const unsigned char *contents, unsigned int colour, unsigned char attr, int x, int y);
};

#endif // SNAKEPIT_H

