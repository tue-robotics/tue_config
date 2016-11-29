cmake_minimum_required(VERSION 2.8.3)
project(tue_config)

find_package(catkin REQUIRED COMPONENTS
  tue_filesystem
  roslib
)

catkin_python_setup()

# ------------------------------------------------------------------------------------------------
#                                          CATKIN EXPORT
# ------------------------------------------------------------------------------------------------

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES tue_config
  CATKIN_DEPENDS tue_filesystem
  DEPENDS YamlCpp
)

# ------------------------------------------------------------------------------------------------
#                                              BUILD
# ------------------------------------------------------------------------------------------------

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

# Check for old YAML-CPP version
if(EXISTS "/usr/include/yaml-cpp/node.h")
  add_definitions(-DYAML_VERSION_0_3)
endif()

set(HEADER_FILES
  include/tue/config/types.h
  include/tue/config/variant.h
  include/tue/config/yaml_emitter.h
  include/tue/config/reader.h
  include/tue/config/writer.h
  include/tue/config/sequence.h
  include/tue/config/node.h
  include/tue/config/map.h
  include/tue/config/value.h
  include/tue/config/data.h
  include/tue/config/label.h
  include/tue/config/binary_writer.h
  include/tue/config/binary_reader.h
  include/tue/config/reader_writer.h
  include/tue/config/configuration.h
  include/tue/config/loaders/yaml.h
  include/tue/config/data_pointer.h
  include/tue/config/read.h
  include/tue/config/write.h
)

add_library(tue_config
  src/data.cpp
  src/node.cpp
  src/map.cpp
  src/sequence.cpp
  src/reader.cpp
  src/writer.cpp
  src/yaml_emitter.cpp
  src/binary_writer.cpp
  src/binary_reader.cpp
  src/reader_writer.cpp
  src/loaders/yaml.cpp
  src/loaders/resolve_functions.cpp
  src/data_pointer.cpp
  src/read.cpp
  src/write.cpp
  ${HEADER_FILES}
)
target_link_libraries(tue_config ${catkin_LIBRARIES} yaml-cpp ${roslib_LIBRARIES})

add_executable(test_tue_config test/test.cpp)
target_link_libraries(test_tue_config tue_config)

add_executable(test_read_and_show test/test_read_and_show.cpp)
target_link_libraries(test_read_and_show tue_config)
