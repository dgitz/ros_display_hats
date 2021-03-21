/*! \file DisplayHat.h
 */
#ifndef ROSHATS_DISPLAYHAT_H
#define ROSHATS_DISPLAYHAT_H
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <ros_display_hats/srv_get_displayinfo.h>
#include <ros_hats/Hat/Hat.h>
#include <sensor_msgs/image_encodings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

#include "Driver/TFTHat/TFTHatDriver.h"
/*! \class DisplayHat
    \brief DisplayHat class
    Basic container for a DisplayHat
*/
class DisplayHat : public Hat
{
   public:
    enum class HatModel {
        UNKNOWN = 0, /*!< Uninitialized value. */
        PITFT_TOUCH =
            1,           /*!< PITFT Hat with Touch Support.  Example:
                            https://learn.adafruit.com/adafruit-pitft-3-dot-5-touch-screen-for-raspberry-pi */
        END_OF_LIST = 2, /*!< Last item of list. Used for Range Checks. */
    };
    //! Convert DisplayHat::HatModel to human readable string
    /*!
      \param v DisplayHat::HatModel type
      \return The converted string.
    */
    static std::string HatModelString(DisplayHat::HatModel v) {
        switch (v) {
            case DisplayHat::HatModel::UNKNOWN: return "UNKNOWN"; break;
            case DisplayHat::HatModel::PITFT_TOUCH: return "PITFT_TOUCH"; break;
            default: return HatModelString(DisplayHat::HatModel::UNKNOWN); break;
        }
    }
    static HatModel HatModelType(std::string v) {
        if (v == "PITFT_TOUCH") {
            return DisplayHat::HatModel::PITFT_TOUCH;
        }
        else {
            return DisplayHat::HatModel::UNKNOWN;
        }
    }
    DisplayHat(HatModel _model) : model(_model), it_(nh_) {
    }
    ~DisplayHat();
    bool init(Logger *_logger, RaspberryPiDefinition::RaspberryPiModel _board, HatConfig _config);
    bool init_ros(boost::shared_ptr<ros::NodeHandle>, std::string host_name);
    void ImageCallback(const sensor_msgs::ImageConstPtr &msg);
    bool displayinfo_service(ros_display_hats::srv_get_displayinfo::Request &req,
                             ros_display_hats::srv_get_displayinfo::Response &res);
    std::string pretty(std::string pre);
    bool update(double dt);

    bool cleanup();

   private:
    HatModel model;
    HatConfig hat_config;
    TFTHatDriver *tfthat_driver;
    ros::NodeHandle nh_;
    ros::ServiceServer displayinfo_srv;
    image_transport::ImageTransport it_;
    image_transport::Subscriber image_sub;
};
#endif  // ROSHATS_DISPLAYHAT_H