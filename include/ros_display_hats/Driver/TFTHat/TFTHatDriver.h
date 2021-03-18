#ifndef TFTHATDRIVER_H
#define TFTHATDRIVER_H
#include <eros/Logger.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

class TFTHatDriver
{
   public:
    // default framebuffer palette
    typedef enum {
        BLACK = 0,         /*   0,   0,   0 */
        BLUE = 1,          /*   0,   0, 172 */
        GREEN = 2,         /*   0, 172,   0 */
        CYAN = 3,          /*   0, 172, 172 */
        RED = 4,           /* 172,   0,   0 */
        PURPLE = 5,        /* 172,   0, 172 */
        ORANGE = 6,        /* 172,  84,   0 */
        LTGREY = 7,        /* 172, 172, 172 */
        GREY = 8,          /*  84,  84,  84 */
        LIGHT_BLUE = 9,    /*  84,  84, 255 */
        LIGHT_GREEN = 10,  /*  84, 255,  84 */
        LIGHT_CYAN = 11,   /*  84, 255, 255 */
        LIGHT_RED = 12,    /* 255,  84,  84 */
        LIGHT_PURPLE = 13, /* 255,  84, 255 */
        YELLOW = 14,       /* 255, 255,  84 */
        WHITE = 15         /* 255, 255, 255 */
    } COLOR_INDEX_T;

    TFTHatDriver();
    ~TFTHatDriver();
    bool init(Logger* _logger, std::string device_fd = "/dev/fb1");
    void cleanup();
    void drawSquare(int x, int y, int height, int width, int c);

   private:
    void put_pixel_16bpp(int x, int y, int r, int g, int b);

    Logger* logger;
    int fd;
    struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo orig_var;
    struct fb_var_screeninfo var;
    long int screensize;
    char* fbp;
    unsigned short def_r[16];
    unsigned short def_g[16];
    unsigned short def_b[16];
};
#endif  // TFTHATDRIVER_H