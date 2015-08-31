cmake_minimum_required(VERSION 2.8.3)
project(tue_config)

find_package(catkin REQUIRED COMPONENTS
    tue_filesystem
)

find_package(roslib)

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
    ${HEADER_FILES}
)
target_link_libraries(tue_config ${catkin_LIBRARIES} yaml-cpp ${roslib_LIBRARIES})

# ------------------------------------------------------------------------------------------------
#                                           TUE_CONFIG2
# ------------------------------------------------------------------------------------------------

set(HEADER_FILES2
    include/tue/config2/error.h
    include/tue/config2/data.h
    include/tue/config2/reader.h
    include/tue/config2/data_reader.h
    include/tue/config2/data_writer.h
    include/tue/config2/yaml.h
    include/tue/config2/writer.h
    include/tue/config2/json_writer.h
    include/tue/config2/json.h
    include/tue/config2/emitter.h
)

add_library(tue_config2
    src/config2/json.cpp
    src/config2/data_writer.cpp
    src/config2/writer.cpp
    src/config2/yaml.cpp
    src/config2/json_writer.cpp
    src/config2/reader.cpp
    src/config2/data_reader.cpp
    src/config2/error.cpp
    src/config2/emitter.cpp
    ${HEADER_FILES2}
)

target_link_libraries(tue_config2 ${catkin_LIBRARIES} yaml-cpp ${roslib_LIBRARIES})

# ------------------------------------------------------------------------------------------------
#                                              TESTS
# ------------------------------------------------------------------------------------------------

add_executable(test_tue_config test/test.cpp)
target_link_libraries(test_tue_config tue_config)

add_executable(test_read_and_show test/test_read_and_show.cpp)
target_link_libraries(test_read_and_show tue_config)
