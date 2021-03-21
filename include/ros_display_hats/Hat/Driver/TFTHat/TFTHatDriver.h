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

#include <boost/algorithm/string.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "opencv2/opencv.hpp"

class TFTHatDriver
{
   public:
    enum class ColorSpace {
        UNKNOWN = 0,
        RGB8 = 1,
        END_OF_LIST = 2,
    };
    struct DisplayInfo {
        DisplayInfo() : width(0), height(0) {
        }
        uint16_t width;
        uint16_t height;
        ColorSpace colorSpace;
        double update_rate_hz;
    };
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
        WHITE = 15,        /* 255, 255, 255 */
        END_OF_LIST = 16
    } COLOR_INDEX_T;

    TFTHatDriver();
    ~TFTHatDriver();
    bool init(Logger* _logger, std::string device_fd = "/dev/fb0");
    void clear_screen();
    bool finish();
    bool new_image(cv::Mat img);
    DisplayInfo get_display_info() {
        return displayInfo;
    }
    void set_backlight(bool v);

   private:
    void put_pixel_16bpp(int x, int y, int r, int g, int b);
    std::string exec(const char* cmd, bool wait_for_result);
    bool initialized;
    DisplayInfo displayInfo;
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
    cv::Mat frame;
    uint64_t update_count;
    bool backlight_on;
};
#endif  // TFTHATDRIVER_H