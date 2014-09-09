cmake_minimum_required(VERSION 2.8.3)
project(tue_config)

find_package(catkin REQUIRED COMPONENTS
  tue_filesystem
)

find_package(PkgConfig)
pkg_check_modules(YamlCpp yaml-cpp)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES tue_config
  CATKIN_DEPENDS tue_filesystem
  DEPENDS YamlCpp
)

###########
## Build ##
###########

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

# to show header files in Qt Creator
file(GLOB_RECURSE HEADER_FILES include/*.h src/*.h)

add_library(tue_config
   src/configuration.cpp
   ${HEADER_FILES}
)

target_link_libraries(tue_config
   ${catkin_LIBRARIES}
   yaml-cpp
)

add_executable(tue_config_test_config test/test_config.cpp)
add_executable(tue_config_test_variant test/test_variant.cpp)

target_link_libraries(tue_config_test_config tue_config)
target_link_libraries(tue_config_test_variant tue_config)
