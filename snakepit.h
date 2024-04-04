#ifndef SNAKEPIT_H
#define SNAKEPIT_H

class Game
{
public:
    void run();

private:
    void init_player();
    void init_snakes();
    void init_snake_pit();
    void render_snake_pit();
};

#endif // SNAKEPIT_H

