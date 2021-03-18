if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL armv7l)
add_library(driver_tfthat TFTHatDriver.cpp)
target_link_libraries(driver_tfthat ${catkin_LIBRARIES} wiringPi logger)

add_executable(test_tfthatdriver test/test_tfthat_driver.cpp)
target_link_libraries(test_tfthatdriver driver_tfthat)
endif ()
