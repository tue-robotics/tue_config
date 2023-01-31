cmake_minimum_required(VERSION 3.0.2)
project(tue_config)

add_compile_options(-Wall -Werror=all)
add_compile_options(-Wextra -Werror=extra)

find_package(Boost REQUIRED COMPONENTS)
find_package(yaml-cpp REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  cmake_modules
  rosconsole_bridge
  roslib
  tue_filesystem
)
find_package(TinyXML2 REQUIRED)

catkin_python_setup()

# ------------------------------------------------------------------------------------------------
#                                          CATKIN EXPORT
# ------------------------------------------------------------------------------------------------

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  DEPENDS Boost TinyXML2
)

# ------------------------------------------------------------------------------------------------
#                                              BUILD
# ------------------------------------------------------------------------------------------------

include_directories(
  include
  SYSTEM
  ${Boost_INCLUDE_DIRS}
  ${TinyXML2_INCLUDE_DIRS}
  ${YAML_CPP_INCLUDE_DIR}
  ${catkin_INCLUDE_DIRS}
)

file(GLOB_RECURSE HEADER_FILES include/*.h)

add_library(${PROJECT_NAME}
  ${HEADER_FILES}
  src/binary_reader.cpp
  src/binary_writer.cpp
  src/data.cpp
  src/data_pointer.cpp
  src/loaders/loader_functions.cpp
  src/loaders/loader_functions.h
  src/loaders/resolve_functions.cpp
  src/loaders/resolve_functions.h
  src/loaders/sdf.cpp
  src/loaders/xml.cpp
  src/loaders/yaml.cpp
  src/map.cpp
  src/node.cpp
  src/read.cpp
  src/reader.cpp
  src/reader_writer.cpp
  src/sequence.cpp
  src/write.cpp
  src/writer.cpp
  src/yaml_emitter.cpp
)
target_link_libraries(${PROJECT_NAME} ${TinyXML2_LIBRARIES} ${YAML_CPP_LIBRARIES} ${catkin_LIBRARIES})

add_executable(test_${PROJECT_NAME} test/test.cpp)
target_link_libraries(test_${PROJECT_NAME} ${PROJECT_NAME})

add_executable(test_yaml_and_show test/test_yaml_and_show.cpp)
target_link_libraries(test_yaml_and_show ${PROJECT_NAME})

add_executable(test_xml_and_show test/test_xml_and_show.cpp)
target_link_libraries(test_xml_and_show ${PROJECT_NAME})

add_executable(test_sdf_and_show test/test_sdf_and_show.cpp)
target_link_libraries(test_sdf_and_show ${PROJECT_NAME})

#############
## Install ##
#############

catkin_install_python(
  PROGRAMS tools/generate-config
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(
  DIRECTORY include/
  DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION}
)

install(
  TARGETS
    ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

install(
  TARGETS
    test_sdf_and_show
    test_${PROJECT_NAME}
    test_xml_and_show
    test_yaml_and_show
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

#############
## Testing ##
#############

if (CATKIN_ENABLE_TESTING)
  find_package(catkin_lint_cmake REQUIRED)
  catkin_add_catkin_lint_test("-W2 --ignore HEADER_OUTSIDE_PACKAGE_INCLUDE_PATH")

  catkin_add_gtest(sdf_gtest test/sdf_gtest.cpp)
  target_link_libraries(sdf_gtest ${PROJECT_NAME})
endif()
