#include <ros_display_hats/Hat/Driver/TFTHat/TFTHatDriver.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <opencv2/imgcodecs.hpp>
using namespace std;

static void show_usage() {
    std::cerr
        << "This program is used to test the operation of a directly connected TFT Display Hat.\n"
        << std::endl;
}
/**********************************************************
Housekeeping variables
 ***********************************************************/

/**********************************************************
Declare Functions
 ***********************************************************/

int main(int argc, char* argv[]) {
    Logger* logger = new Logger("DEBUG", "TFTHatDriverTest");
    TFTHatDriver* driver = new TFTHatDriver();
    bool v = driver->init(logger);
    TFTHatDriver::DisplayInfo display_info = driver->get_display_info();
    logger->log_notice("Width: " + std::to_string(display_info.width) +
                       " Height: " + std::to_string(display_info.height));

    if (v == false) {
        logger->log_error("Unable to initialize Driver. Exiting.");
        return -1;
    }
    TFTHatDriver::DisplayInfo displayInfo = driver->get_display_info();
    if ((displayInfo.width == 0) || (displayInfo.height == 0)) {
        logger->log_error("Screen Size error.");
        return -1;
    }
    std::string dir = "~/catkin_ws/src/ros_display_hats/src/Hat/Driver/TFTHat/test/media/";

    double dt = 0.5;
    int counter = 0;
    while (counter < 60) {
        for (int i = 0; i < 5; ++i) {
            std::string image_name = "image_00" + std::to_string(i) + ".jpg";
            std::string image_path = dir + image_name;
            cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);
            if (img.empty()) {
                logger->log_error("Failed to load image: " + image_path);
            }
            cv::resize(
                img, img, cv::Size(display_info.width, display_info.height), 0, 0, CV_INTER_LINEAR);
            v = driver->new_image(img);
            if (v == false) {
                logger->log_error("Unable to process new image.");
            }
            else {
                logger->log_debug("Displayed Image: " + image_path);
            }
            usleep((1.0 / display_info.update_rate_hz) * 1000000.0);
            counter++;
        }
    }
    /*
    int counter = 0;
    double dt = 0.5;
    int x = 2;
    int step_x = (int)(((double)displayInfo.width) / 50.0);
    int y = 2;
    int step_y = (int)(((double)displayInfo.height) / 50.0);
    int width = displayInfo.width - 2;
    int height = displayInfo.height - 2;
    int color = TFTHatDriver::BLUE;
    while (counter < 5) {
        usleep(dt * 1000000.0);

        driver->drawSquare(x, y, width, height, color);

        color++;
        if (color == TFTHatDriver::END_OF_LIST) {
            color = TFTHatDriver::BLUE;
        }
        x += step_x;
        y += step_y;
        width -= (step_x * 2);
        height -= (step_y * 2);
        if ((width < 4) || (height < 4)) {
            x = 2;
            y = 2;
            width = displayInfo.width - 5;
            height = displayInfo.height - 5;
            driver->clear_screen();
            counter++;
            logger->log_notice("Iter: " + std::to_string(counter));
        }
    }
    */
    logger->log_notice("Cleaning Up");
    driver->finish();
    if (logger != nullptr) {
        delete logger;
        logger = nullptr;
    }
    if (driver != nullptr) {
        delete driver;
        driver = nullptr;
    }
    return 0;
}