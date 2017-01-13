cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

project(prometheus-cpp)
enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

find_package(GoogleBenchmark)
find_package(Protobuf REQUIRED)
find_package(Telegraf)

# suppress warnings

if(APPLE)
  string(APPEND CMAKE_CXX_FLAGS " -Wno-deprecated-declarations")
endif()

# civetweb

add_library(civetweb
  3rdparty/civetweb/include/CivetServer.h
  3rdparty/civetweb/include/civetweb.h
  3rdparty/civetweb/src/CivetServer.cpp
  3rdparty/civetweb/src/civetweb.c
  3rdparty/civetweb/src/handle_form.inl
  3rdparty/civetweb/src/md5.inl
)

target_compile_definitions(civetweb PRIVATE
  USE_IPV6
  NDEBUG
  NO_CGI
  NO_CACHING
  NO_SSL
  NO_FILES
)

target_include_directories(civetweb PUBLIC
  3rdparty/civetweb/include
)

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

# prometheus-cpp

add_subdirectory(lib)
add_subdirectory(tests)
