cmake_minimum_required(VERSION 2.8.3)
project(tue_config)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  tue_filesystem
)

## Uncomment this if the package has a setup.py. This macro ensures
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES tue_config
  CATKIN_DEPENDS tue_filesystem
  DEPENDS yaml-cpp
)

###########
## Build ##
###########
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  yaml-cpp
)

# to show header files in Qt Creator
file(GLOB_RECURSE HEADER_FILES include/*.h src/*.h)

# Declare a cpp library
add_library(tue_config
   src/configuration.cpp
   ${HEADER_FILES}
)

target_link_libraries(tue_config
   ${catkin_LIBRARIES}
   yaml-cpp
)

# Declare a cpp executable
add_executable(tue_config_test_config test/test_config.cpp)
add_executable(tue_config_test_variant test/test_variant.cpp)

target_link_libraries(tue_config_test_config tue_config)
target_link_libraries(tue_config_test_variant tue_config)

