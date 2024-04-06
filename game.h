#ifndef GAME_H
#define GAME_H

void KeyPressedHandler(const char *string);
void KeyReleasedHandler(const char *string);
void KeyboardRemovedHandler(CDevice *device, void *context);
void KeyboardShutdownHandler();

class Game
{
public:
    Game(CDeviceNameService *deviceNameService,
         CScreenDevice *screen,
         CTimer *timer,
         CUSBHCIDevice *usbDevice);
    bool init();
    void go();

private:
    CDeviceNameService *deviceNameService;
    CScreenDevice *screen;
    CTimer *timer;
    CUSBHCIDevice *usbDevice;
    CUSBKeyboardDevice * volatile keyboard;
    Player* player;
    Snake** snake;
    void get_keyboard(bool waitTilFound);
    bool get_keyboard();
    void reset_game();
    Player* create_player();
    void init_player();
    Snake** create_snakes();
    void init_snakes();
    void reset_player();
    void reset_snakes();
    void init_snake_pit();
    void render_snake_pit();
    void render_cell(Point cellPos);
    void render_cells(Point *cellPos, unsigned int num);
    void draw_char(const unsigned char *contents, unsigned int colour, unsigned char attr, int x, int y);
};

#endif // GAME_H