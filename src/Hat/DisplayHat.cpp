#include <ros_display_hats/Hat/DisplayHat.h>
DisplayHat::~DisplayHat() {
}
std::string DisplayHat::pretty(std::string pre) {
    std::string str = base_pretty(pre);
    str += "Type: DisplayHat Model: " + HatModelString(model) + " --- \n";
    return str;
}
bool DisplayHat::init(Logger* _logger,
                      RaspberryPiDefinition::RaspberryPiModel _board,
                      HatConfig _config) {
    bool v = base_init(_logger, _board);
    if (v == false) {
        return false;
    }

    if ((model == HatModel::UNKNOWN) || (model == HatModel::END_OF_LIST)) {
        return false;
    }

    hat_config = _config;
    diagnostic.device_name = hat_config.hat_name;
    diagnostic.node_name = hat_config.hat_name;
    diagnostic.system = System::MainSystem::ROVER;
    diagnostic.subsystem = System::SubSystem::ROBOT_CONTROLLER;
    diagnostic.component = System::Component::POSE;
    diagnostic.type = Diagnostic::DiagnosticType::SENSORS;
    diagnostic.message = Diagnostic::Message::INITIALIZING;
    diagnostic.level = Level::Type::INFO;
    diagnostic.description = "Hat Initializing";
    diag_helper.initialize(diagnostic);
    diag_helper.enable_diagnostics(std::vector<Diagnostic::DiagnosticType>{diagnostic.type});
    diagnostic = diag_helper.update_diagnostic(diagnostic);
    if (model == DisplayHat::HatModel::PITFT_TOUCH) {
        if (hat_config.use_default_config == true) {
            logger->log_warn("[DisplayHat] Using Default Values for Model: " +
                             DisplayHat::HatModelString(model));
        }
        else {
            logger->log_error("Only default config currently supported. Exiting.");
            return false;
        }
    }
    else {
        logger->log_error("Model: " + DisplayHat::HatModelString(model) +
                          " Not Supported. Exiting.");
        return false;
    }
    if (model == HatModel::PITFT_TOUCH) {
        tfthat_driver = new TFTHatDriver();
        v = tfthat_driver->init(logger);

        if (v == false) {
            diagnostic =
                diag_helper.update_diagnostic(Diagnostic::DiagnosticType::SENSORS,
                                              Level::Type::ERROR,
                                              Diagnostic::Message::INITIALIZING_ERROR,
                                              "Unable to initialize TFTHat Driver.  Exiting.");
            logger->log_diagnostic(diagnostic);
            return false;
        }
    }
    diagnostic = diag_helper.update_diagnostic(Diagnostic::DiagnosticType::SENSORS,
                                               Level::Type::INFO,
                                               Diagnostic::Message::NOERROR,
                                               "Initialized Hat.");

    return true;
}
void DisplayHat::ImageCallback(const sensor_msgs::ImageConstPtr& msg) {
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        if (model == HatModel::PITFT_TOUCH) {
            TFTHatDriver::DisplayInfo displayInfo = tfthat_driver->get_display_info();
            cv::Mat img;
            cv::resize(cv_ptr->image,
                       img,
                       cv::Size(displayInfo.width, displayInfo.height),
                       0,
                       0,
                       CV_INTER_LINEAR);
            if (tfthat_driver->new_image(img) == false) {
                logger->log_warn("Failed to Update Display.");
            }
            else {
                logger->log_debug("Updated Display.");
            }
        }
    }
    catch (cv_bridge::Exception& e) {
        logger->log_warn("Image Exception: " + std::string(e.what()));
        return;
    }
}
bool DisplayHat::displayinfo_service(ros_display_hats::srv_get_displayinfo::Request& req,
                                     ros_display_hats::srv_get_displayinfo::Response& res) {
    return false;
}
bool DisplayHat::init_ros(boost::shared_ptr<ros::NodeHandle> _n, std::string host_name) {
    logger->log_debug("[DisplayHat] Initializing ROS...");
    if (_n == nullptr) {
        logger->log_error("Node Handle has No Memory.");
        return false;
    }
    nodeHandle = _n;
    std::string srv_displayinfo_topic = "/srv_get_displayinfo";
    displayinfo_srv =
        nodeHandle->advertiseService(srv_displayinfo_topic, &DisplayHat::displayinfo_service, this);
    it_ = image_transport::ImageTransport(*nodeHandle.get());
    std::string tempstr = "/" + host_name + "/" + hat_config.hat_name;
    image_sub = it_.subscribe(tempstr, 1, &DisplayHat::ImageCallback, this);
    ros_initialized = true;
    logger->log_debug("[DisplayHat] ROS Initialized.");
    return true;
}
bool DisplayHat::update(double dt) {
    bool v = false;
    /*
    bool v = driver->update(dt);
    if (v == true) {
        }
    }
    */

    return v;
}
bool DisplayHat::cleanup() {
    bool cleanup_ok = true;
    if (model == HatModel::PITFT_TOUCH) {
        bool v = tfthat_driver->finish();
        if (v == false) {
            cleanup_ok = false;
        }
        if (tfthat_driver != nullptr) {
            delete tfthat_driver;
        }
        tfthat_driver = nullptr;
    }
    if (cleanup_ok == true) {
        logger->log_notice("[DisplayHat] Cleaned Up Successfully.");
    }
    else {
        logger->log_warn("[DisplayHat] Cleanup Failed.");
    }
    return cleanup_ok;
}