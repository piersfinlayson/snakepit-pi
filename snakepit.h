#ifndef SNAKEPIT_H
#define SNAKEPIT_H

#include <circle/logger.h>
#include <circle/screen.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/usb/usbhcidevice.h>
#include <circle/usb/usbkeyboard.h>
#include <circle/timer.h>
#include <circle/bcmrandom.h>

static const char FromSnakepit[] = "snakepit";

class Game
{
public:
    CLogger logger;
    Game(CLogger logger, CScreenDevice screen, CDeviceNameService deviceNameService, CUSBHCIDevice usbhci, CTimer timer);
    void init();
    void run();

private:
    CScreenDevice screen;
    CDeviceNameService deviceNameService;
    CUSBHCIDevice usbhci;
    CUSBKeyboardDevice * volatile keyboard;
    CTimer timer;
    void get_keyboard();
    void init_player();
    void init_snakes();
    void init_snake_pit();
    void render_snake_pit();
    void draw_char(const unsigned char *contents, unsigned int colour, unsigned char attr, int x, int y);
};

#endif // SNAKEPIT_H

