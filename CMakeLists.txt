cmake_minimum_required(VERSION 3.5)
project(tue_config)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Werror=all)
  add_compile_options(-Wextra -Werror=extra)
endif()

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS)
find_package(console_bridge REQUIRED)
find_package(TinyXML2 QUIET CONFIG)
if(NOT TinyXML2_FOUND)
  # Ubuntu 22.04 (Humble) does not ship a CMake config for tinyxml2 and the
  # build image does not provide pkg-config either, so locate the library
  # manually and create the conventional imported target.
  find_path(TinyXML2_INCLUDE_DIR NAMES tinyxml2.h)
  find_library(TinyXML2_LIBRARY NAMES tinyxml2)
  if(NOT TinyXML2_INCLUDE_DIR OR NOT TinyXML2_LIBRARY)
    message(FATAL_ERROR
      "Could not find TinyXML2 (headers: ${TinyXML2_INCLUDE_DIR}, "
      "library: ${TinyXML2_LIBRARY})")
  endif()
  add_library(tinyxml2::tinyxml2 UNKNOWN IMPORTED)
  set_target_properties(tinyxml2::tinyxml2 PROPERTIES
    IMPORTED_LOCATION "${TinyXML2_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${TinyXML2_INCLUDE_DIR}")
  set(TinyXML2_FOUND TRUE)
endif()
find_package(yaml_cpp_vendor REQUIRED)
find_package(yaml-cpp REQUIRED)
# yaml-cpp 0.7 (Ubuntu 22.04 / Humble) exports the bare `yaml-cpp` target,
# while yaml-cpp >= 0.8 exports the namespaced `yaml-cpp::yaml-cpp` target.
# Provide an alias for the older version so we can always link against the
# namespaced name.
if(NOT TARGET yaml-cpp::yaml-cpp AND TARGET yaml-cpp)
  add_library(yaml-cpp::yaml-cpp ALIAS yaml-cpp)
endif()

# to show header files in Qt Creator
file(GLOB_RECURSE HEADER_FILES include/*.h)

# ------------------------------------------------------------------------------------------------
#                                              BUILD
# ------------------------------------------------------------------------------------------------

add_library(${PROJECT_NAME} SHARED
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

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
  ${Boost_INCLUDE_DIRS}
  ${console_bridge_INCLUDE_DIRS}
  ${YAML_CPP_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
  ament_index_cpp::ament_index_cpp
  ${Boost_LIBRARIES}
  ${console_bridge_LIBRARIES}
  tinyxml2::tinyxml2
  yaml-cpp::yaml-cpp
)

# ------------------------------------------------------------------------------------------------
#                                              TOOLS
# ------------------------------------------------------------------------------------------------

add_executable(test_${PROJECT_NAME} test/test.cpp)
target_link_libraries(test_${PROJECT_NAME} ${PROJECT_NAME})

add_executable(test_yaml_and_show test/test_yaml_and_show.cpp)
target_link_libraries(test_yaml_and_show ${PROJECT_NAME})

add_executable(test_xml_and_show test/test_xml_and_show.cpp)
target_link_libraries(test_xml_and_show ${PROJECT_NAME})

add_executable(test_sdf_and_show test/test_sdf_and_show.cpp)
target_link_libraries(test_sdf_and_show ${PROJECT_NAME})

# ------------------------------------------------------------------------------------------------
#                                              INSTALL
# ------------------------------------------------------------------------------------------------

ament_python_install_package(${PROJECT_NAME} PACKAGE_DIR src/${PROJECT_NAME})

install(
  PROGRAMS tools/generate-config
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  FILES test/model.sdf
  DESTINATION share/${PROJECT_NAME}/test
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  TARGETS
    test_${PROJECT_NAME}
    test_sdf_and_show
    test_xml_and_show
    test_yaml_and_show
  DESTINATION lib/${PROJECT_NAME}
)

# ------------------------------------------------------------------------------------------------
#                                              EXPORT
# ------------------------------------------------------------------------------------------------

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(ament_index_cpp Boost console_bridge TinyXML2 yaml-cpp)

# ------------------------------------------------------------------------------------------------
#                                           UNIT TESTS
# ------------------------------------------------------------------------------------------------

if(BUILD_TESTING)
  find_package(tue_lint_config REQUIRED)
  find_package(ament_cmake_clang_format REQUIRED)
  find_package(ament_cmake_clang_tidy REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_cmake_lint_cmake REQUIRED)
  find_package(ament_cmake_xmllint REQUIRED)

  ament_clang_format(
    CONFIG_FILE ${tue_lint_config_DIR}/../config/.clang-format
    --clang-format-version=21
  )
  ament_clang_tidy(
    CONFIG_FILE ${tue_lint_config_DIR}/../config/.clang-tidy
    --clang-tidy-version=21
    ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/compile_commands.json)
  ament_lint_cmake(MAX_LINE_LENGTH 120 "--filter=
    +convention/filename,
    +linelength,
    +package/consistency,
    +package/stdargs,
    +readability/logic,
    +readability/mixedcase,
    +readability/wonkycase,
    +syntax,+whitespace/eol,
    +whitespace/extra,
    +whitespace/indent,
    +whitespace/mismatch,
    +whitespace/newline,
    +whitespace/tabs"
  )
  ament_xmllint(MAX_LINE_LENGTH 120)

  ament_add_gtest(sdf_gtest test/sdf_gtest.cpp)
  target_link_libraries(sdf_gtest ${PROJECT_NAME})
endif()

ament_package()
