cmake_minimum_required(VERSION 3.0.2)
project(ros_display_hats)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  eros
  ros_hats
  cv_bridge
  image_transport
)
find_package(OpenCV)
add_service_files(
  FILES
  srv_get_displayinfo.srv
)
generate_messages(DEPENDENCIES std_msgs)
catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS eros ros_hats
)

include_directories(
 include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}

)
add_subdirectory(src)
add_subdirectory(nodes)

