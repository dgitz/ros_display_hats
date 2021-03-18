#include <ros_display_hats/Driver/TFTHat/TFTHatDriver.h>
TFTHatDriver::TFTHatDriver()
    : logger(nullptr),
      fd(0),
      screensize(0),
      fbp(0),
      def_r{0, 0, 0, 0, 172, 172, 172, 168, 84, 84, 84, 84, 255, 255, 255, 255},
      def_g{0, 0, 168, 168, 0, 0, 84, 168, 84, 84, 255, 255, 84, 84, 255, 255},
      def_b{0, 172, 0, 168, 0, 172, 0, 168, 84, 255, 84, 255, 84, 255, 84, 255} {
}
TFTHatDriver::~TFTHatDriver() {
}
bool TFTHatDriver::init(Logger* _logger, std::string device_fd) {
    if (_logger == nullptr) {
        return false;
    }
    logger = _logger;

    fd = open(device_fd.c_str(), O_RDWR);
    if (fd == -1) {
        logger->log_error("Unable to open: " + device_fd);
        return false;
    }

    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0) {
        logger->log_error("ioctl error: FBIOGET_FSCREENINFO on device.");
        close(fd);
        return false;
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0) {
        logger->log_error("ioctl error: FBIOGET_VSCREENINFO on device.");
        close(fd);
        return false;
    }
    memcpy(&orig_var, &var, sizeof(struct fb_var_screeninfo));
    screensize = fix.smem_len;
    fbp = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ((int)fbp == -1) {
        logger->log_error("Failed to mmap");
    }

    // clear framebuffer
    int x, y;
    for (x = 0; x < var.xres; x++) {
        for (y = 0; y < var.yres; y++) { put_pixel_16bpp(x, y, 0, 0, 0); }
    }

    return true;
}
void TFTHatDriver::cleanup() {
    int x, y;
    for (x = 0; x < var.xres; x++)
        for (y = 0; y < var.yres; y++) put_pixel_16bpp(x, y, 0, 0, 0);

    munmap(fbp, screensize);
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &orig_var)) {
        logger->log_error("Error re-setting variable information.");
    }
    close(fd);
}
void TFTHatDriver::put_pixel_16bpp(int x, int y, int r, int g, int b) {
    unsigned int pix_offset;
    unsigned short c;

    // calculate the pixel's byte offset inside the buffer
    pix_offset = x * 2 + y * fix.line_length;

    // some magic to work out the color
    c = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);

    // write 'two bytes at once'
    *((unsigned short*)(fbp + pix_offset)) = c;
}
void TFTHatDriver::drawSquare(int x, int y, int height, int width, int c) {
    int h = 0;
    int w = 0;
    for (h = 0; h < height; h++)
        for (w = 0; w < width; w++)
            put_pixel_16bpp(h + (x - 2), w + (y - 2), def_r[c], def_g[c], def_b[c]);
}