cmake_minimum_required(VERSION 2.8.12.2 FATAL_ERROR)
if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

project(prometheus-cpp)
enable_testing()
include(GNUInstallDirs)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)

if(NOT DEFINED CMAKE_CXX_STANDARD AND UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads)

find_package(GoogleBenchmark)
find_package(Telegraf)

# suppress warnings

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

# civetweb

set(CIVETWEB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/3rdparty/civetweb/include)

add_library(civetweb OBJECT
  3rdparty/civetweb/include/CivetServer.h
  3rdparty/civetweb/include/civetweb.h
  3rdparty/civetweb/src/CivetServer.cpp
  3rdparty/civetweb/src/civetweb.c
  3rdparty/civetweb/src/handle_form.inl
  3rdparty/civetweb/src/md5.inl
)

target_compile_definitions(civetweb PRIVATE
  CIVETWEB_API=
  USE_IPV6
  NDEBUG
  NO_CGI
  NO_CACHING
  NO_SSL
  NO_FILES
)

target_include_directories(civetweb PUBLIC
  ${CIVETWEB_INCLUDE_DIR}
)

if(BUILD_SHARED_LIBS)
  set_target_properties(civetweb PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
  )
endif()

# google mock

add_library(gmock_main STATIC EXCLUDE_FROM_ALL
  3rdparty/googletest/googletest/src/gtest-all.cc
  3rdparty/googletest/googlemock/src/gmock-all.cc
  3rdparty/googletest/googlemock/src/gmock_main.cc
)

target_include_directories(gmock_main
  PUBLIC
    3rdparty/googletest/googletest/include
    3rdparty/googletest/googlemock/include
  PRIVATE
    3rdparty/googletest/googletest
    3rdparty/googletest/googlemock
)

target_link_libraries(gmock_main PRIVATE
  ${CMAKE_THREAD_LIBS_INIT}
)

# prometheus-cpp

add_subdirectory(core)
add_subdirectory(pull)

# install

set(CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/prometheus-cpp-config.cmake.in prometheus-cpp-config.cmake
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )
install(EXPORT prometheus-cpp-targets DESTINATION ${CMAKECONFIG_INSTALL_DIR} NAMESPACE prometheus-cpp::)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp-config.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})
