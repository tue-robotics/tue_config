cmake_minimum_required(VERSION 3.0.2)
project(tue_config)

find_package(catkin REQUIRED COMPONENTS
  cmake_modules
  tue_filesystem
  roslib
)

find_package(TinyXML2 REQUIRED)

catkin_python_setup()

# ------------------------------------------------------------------------------------------------
#                                          CATKIN EXPORT
# ------------------------------------------------------------------------------------------------

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES tue_config
  DEPENDS TinyXML2
)

# ------------------------------------------------------------------------------------------------
#                                              BUILD
# ------------------------------------------------------------------------------------------------

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${TinyXML2_INCLUDE_DIRS}
)

file(GLOB_RECURSE HEADER_FILES include/*.h)

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
  src/loaders/sdf.cpp
  src/loaders/xml.cpp
  src/loaders/yaml.cpp
  src/loaders/loader_functions.cpp
  src/loaders/loader_functions.h
  src/loaders/resolve_functions.cpp
  src/loaders/resolve_functions.h
  src/data_pointer.cpp
  src/read.cpp
  src/write.cpp
  ${HEADER_FILES}
)
target_link_libraries(tue_config ${catkin_LIBRARIES} yaml-cpp ${roslib_LIBRARIES} ${TinyXML2_LIBRARIES})

add_executable(test_tue_config test/test.cpp)
target_link_libraries(test_tue_config tue_config)

add_executable(test_yaml_and_show test/test_yaml_and_show.cpp)
target_link_libraries(test_yaml_and_show tue_config)

add_executable(test_xml_and_show test/test_xml_and_show.cpp)
target_link_libraries(test_xml_and_show tue_config)

add_executable(test_sdf_and_show test/test_sdf_and_show.cpp)
target_link_libraries(test_sdf_and_show tue_config)

if (CATKIN_ENABLE_TESTING)
  catkin_add_gtest(sdf_gtest test/sdf_gtest.cpp)
  target_link_libraries(sdf_gtest tue_config ${catkin_LIBRARIES})
endif()
