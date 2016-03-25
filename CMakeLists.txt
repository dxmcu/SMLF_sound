cmake_minimum_required(VERSION 2.8.3)
project(sound_server)

SET(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

find_package(catkin REQUIRED COMPONENTS
  roscpp
  autoscrubber_services
)

catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS
    roscpp
    autoscrubber_services
)


include_directories(
    include
    ${catkin_INCLUDE_DIRS}
)

add_executable(sound_server src/sound_server.cpp)
target_link_libraries(${PROJECT_NAME} sfml-system sfml-audio  ${catkin_LIBRARIES})
add_dependencies(${PROJECT_NAME} ${catkin_EXPORTED_TARGETS})
