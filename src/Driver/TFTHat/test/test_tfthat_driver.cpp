#include <ros_display_hats/Driver/TFTHat/TFTHatDriver.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

    if (v == false) {
        logger->log_error("Unable to initialize Driver. Exiting.");
        return -1;
    }
    while (1) {
        driver->drawSquare(50, 50, 5, 5, TFTHatDriver::WHITE);
        usleep(100000);  // Sleep for 100 mS
    }
    driver->cleanup();
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