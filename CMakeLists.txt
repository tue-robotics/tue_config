cmake_minimum_required(VERSION 2.8.3)
project(tue_config2)

find_package(catkin REQUIRED COMPONENTS
)

# find_package(Boost REQUIRED COMPONENTS system program_options)
# find_package(PCL REQUIRED)
# find_package(OpenCV REQUIRED)

# ------------------------------------------------------------------------------------------------
#                                     ROS MESSAGES AND SERVICES
# ------------------------------------------------------------------------------------------------

# Generate messages
# add_message_files(
#    FILES
#    message1.msg
#    ...
# )

# Generate services
# add_service_files(
#    FILES
#    service1.srv
#    ...
# )

# Generate added messages and services with any dependencies listed here
# generate_messages(
#    DEPENDENCIES
#    geometry_msgs
#    ...
# )

# ------------------------------------------------------------------------------------------------
#                                          CATKIN EXPORT
# ------------------------------------------------------------------------------------------------

catkin_package(
  INCLUDE_DIRS include
#  LIBRARIES bla
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS system_lib
)

# ------------------------------------------------------------------------------------------------
#                                              BUILD
# ------------------------------------------------------------------------------------------------

include_directories(
    include
    ${catkin_INCLUDE_DIRS}
)

set(HEADER_FILES
    include/tue/config2/types.h
    include/tue/config2/variant.h
    include/tue/config2/yaml_emitter.h
    include/tue/config2/reader.h
    include/tue/config2/writer.h
    include/tue/config2/sequence.h
    include/tue/config2/node.h
    include/tue/config2/map.h
    include/tue/config2/value.h
    include/tue/config2/configuration.h
    include/tue/config2/label.h
    include/tue/config2/binary_writer.h
    include/tue/config2/binary_reader.h
)

add_library(tue_config2
    src/configuration.cpp
    src/node.cpp
    src/map.cpp
    src/sequence.cpp
    src/reader.cpp
    src/writer.cpp
    src/yaml_emitter.cpp
    src/binary_writer.cpp
    src/binary_reader.cpp
    ${HEADER_FILES}
)
# target_link_libraries(library_name ${catkin_LIBRARIES})

add_executable(test_tue_config2
	test/test.cpp
)
target_link_libraries(test_tue_config2 tue_config2)


