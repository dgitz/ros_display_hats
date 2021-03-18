add_library(displayHatProcess DisplayHatProcess.cpp)
target_link_libraries(displayHatProcess ${catkin_LIBRARIES} baseNodeProcess)

add_executable(display_hatnode DisplayHatNode.cpp) 
target_link_libraries(display_hatnode ${catkin_LIBRARIES} ${Boost_LIBRARIES} displayHatProcess baseNode)
add_dependencies(display_hatnode ${eros_EXPORTED_TARGETS})

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL armv7l)
    target_link_libraries(display_hatnode driver_tfthat)
endif()
add_subdirectory(test)
