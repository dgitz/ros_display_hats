add_subdirectory(Driver)
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL armv7l)
        add_library(roshat_displayhat DisplayHat.cpp)
        target_link_libraries(roshat_displayhat ${catkin_LIBRARIES} wiringPi logger roshat_hat driver_tfthat)
        install(TARGETS roshat_displayhat 
                ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
                LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
                RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
endif ()
