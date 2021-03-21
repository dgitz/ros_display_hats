#include <ros_display_hats/Hat/Driver/TFTHat/TFTHatDriver.h>
TFTHatDriver::TFTHatDriver()
    : initialized(false),
      logger(nullptr),
      fd(0),
      screensize(0),
      fbp(0),
      def_r{0, 0, 0, 0, 172, 172, 172, 168, 84, 84, 84, 84, 255, 255, 255, 255},
      def_g{0, 0, 168, 168, 0, 0, 84, 168, 84, 84, 255, 255, 84, 84, 255, 255},
      def_b{0, 172, 0, 168, 0, 172, 0, 168, 84, 255, 84, 255, 84, 255, 84, 255},
      update_count(0),
      backlight_on(false) {
}
TFTHatDriver::~TFTHatDriver() {
    if (initialized == true) {
        finish();
    }
}
bool TFTHatDriver::init(Logger* _logger, std::string device_fd) {
    if (_logger == nullptr) {
        return false;
    }
    logger = _logger;
    logger->log_debug("Init Driver");

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
    displayInfo.width = var.xres;
    displayInfo.height = var.yres;
    displayInfo.colorSpace = ColorSpace::RGB8;
    displayInfo.update_rate_hz = 1.0;
    clear_screen();
    set_backlight(false);
    initialized = true;
    logger->log_debug("Driver Initialized.");
    return true;
}
bool TFTHatDriver::finish() {
    set_backlight(false);
    logger->log_debug("Cleaning Up Screen");

    clear_screen();

    munmap(fbp, screensize);
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &orig_var)) {
        logger->log_error("Error re-setting variable information.");
        return false;
    }
    close(fd);
    initialized = false;
    return true;
}
void TFTHatDriver::set_backlight(bool v) {
    if (v == true) {
        exec("echo \"1\" > /sys/class/backlight/soc\\:backlight/brightness", true);
        backlight_on = true;
    }
    else {
        exec("echo \"0\" > /sys/class/backlight/soc\\:backlight/brightness", true);
        backlight_on = false;
    }
}
bool TFTHatDriver::new_image(cv::Mat img) {
    if (!img.data) {
        logger->log_warn("No Image Data!");
        return false;
    }
    if (img.rows != displayInfo.height) {
        logger->log_warn("Image height mismatch.");
        return false;
    }
    if (img.cols != displayInfo.width) {
        logger->log_warn("Image Width mismatch.");
        return false;
    }
    if (update_count == 0) {
        frame = img;
    }

    // We iterate over all pixels of the image
    if (1) {
        for (int r = 0; r < img.rows; r++) {
            // We obtain a pointer to the beginning of row r
            cv::Vec3b* ptr = img.ptr<cv::Vec3b>(r);

            for (int c = 0; c < img.cols; c++) {
                // We invert the blue and red values of the pixel
                ptr[c] = cv::Vec3b(ptr[c][2], ptr[c][1], ptr[c][0]);
                put_pixel_16bpp(c, r, ptr[c][0], ptr[c][1], ptr[c][2]);
            }
        }
    }
    else {
        cv::Mat diff = frame - img;
        uint16_t skipped_counter = 0;
        for (int r = 0; r < diff.rows; r++) {
            // We obtain a pointer to the beginning of row r
            cv::Vec3b* ptr = diff.ptr<cv::Vec3b>(r);

            for (int c = 0; c < diff.cols; c++) {
                if ((ptr[c][0] == 0) && (ptr[c][1] == 0) && (ptr[c][2] == 0)) {
                    skipped_counter++;
                }
                else {
                    cv::Vec3b* n_ptr = img.ptr<cv::Vec3b>(r);
                    n_ptr[c] = cv::Vec3b(n_ptr[c][2], n_ptr[c][1], n_ptr[c][0]);
                    put_pixel_16bpp(c, r, n_ptr[c][0], n_ptr[c][1], n_ptr[c][2]);
                }
            }
        }
        double efficiency =
            100.0 * (double)(skipped_counter) / (double)(displayInfo.height * displayInfo.width);
        logger->log_debug("Eff Gain: " + std::to_string(efficiency) + "%");
    }
    if (backlight_on == false) {
        set_backlight(true);
    }
    update_count++;
    frame = img;
    return true;
}
void TFTHatDriver::clear_screen() {
    for (int x = 0; x < var.xres; x++) {
        for (int y = 0; y < var.yres; y++) { put_pixel_16bpp(x, y, 0, 0, 0); }
    }
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
std::string TFTHatDriver::exec(const char* cmd, bool wait_for_result) {
    char buffer[512];
    std::string result = "";
    try {
        FILE* pipe = popen(cmd, "r");
        if (wait_for_result == false) {
            pclose(pipe);
            return "";
        }
        if (!pipe) {
            std::string tempstr = "popen() failed with command: " + std::string(cmd);
            logger->log_error(tempstr);
            pclose(pipe);
            return "";
        }
        try {
            while (!feof(pipe)) {
                if (fgets(buffer, 512, pipe) != NULL)
                    result += buffer;
            }
        }
        catch (const std::exception& e) {
            pclose(pipe);
            std::string tempstr = "popen() failed with command: " + std::string(cmd) +
                                  " and exception: " + std::string(e.what());
            logger->log_error(tempstr);
            return "";
        }
        pclose(pipe);
        boost::algorithm::trim(result);
        return result;
    }
    catch (const std::exception& e) {
        std::string tempstr = "popen() failed with command: " + std::string(cmd) +
                              " and exception: " + std::string(e.what());
        logger->log_error(tempstr);
        return "";
    }
}