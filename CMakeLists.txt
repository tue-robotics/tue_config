cmake_minimum_required(VERSION 2.8.3)
project(tue_config)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  cmake_modules
  tue_filesystem
  roslib
)

find_package(TinyXML REQUIRED)

catkin_python_setup()

# ------------------------------------------------------------------------------------------------
#                                          CATKIN EXPORT
# ------------------------------------------------------------------------------------------------

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES tue_config
  CATKIN_DEPENDS tue_filesystem
  DEPENDS TinyXML
)

# ------------------------------------------------------------------------------------------------
#                                              BUILD
# ------------------------------------------------------------------------------------------------

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

set(HEADER_FILES
  include/tue/config/types.h
  include/tue/config/variant.h
  include/tue/config/yaml_emitter.h
  include/tue/config/reader.h
  include/tue/config/writer.h
  include/tue/config/sequence.h
  include/tue/config/node.h
  include/tue/config/node_type.h
  include/tue/config/map.h
  include/tue/config/value.h
  include/tue/config/data.h
  include/tue/config/label.h
  include/tue/config/binary_writer.h
  include/tue/config/binary_reader.h
  include/tue/config/reader_writer.h
  include/tue/config/configuration.h
  include/tue/config/loaders/xml.h
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
  src/loaders/xml.cpp
  src/loaders/yaml.cpp
  src/loaders/resolve_functions.cpp
  src/data_pointer.cpp
  src/read.cpp
  src/write.cpp
  ${HEADER_FILES}
)
target_link_libraries(tue_config ${catkin_LIBRARIES} yaml-cpp ${roslib_LIBRARIES} ${TinyXML_LIBRARIES})

add_executable(test_tue_config test/test.cpp)
target_link_libraries(test_tue_config tue_config)

add_executable(test_read_and_show test/test_read_and_show.cpp)
target_link_libraries(test_read_and_show tue_config)

add_executable(test_xml_and_show test/test_xml_and_show.cpp)
target_link_libraries(test_xml_and_show tue_config)
