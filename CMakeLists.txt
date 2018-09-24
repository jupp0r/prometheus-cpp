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

option(ENABLE_PULL "Build prometheus-cpp pull library" ON)
option(ENABLE_PUSH "Build prometheus-cpp push library" ON)
option(ENABLE_COMPRESSION "Enable gzip compression" ON)
option(ENABLE_TESTING "Build tests" ON)
option(USE_THIRDPARTY_LIBRARIES "Use 3rdParty submodules" ON)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads)

if(ENABLE_TESTING)
  find_package(googlemock-3rdparty CONFIG REQUIRED PATHS ${PROJECT_SOURCE_DIR}/cmake)
  find_package(GoogleBenchmark)
endif()

# suppress warnings

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

# prometheus-cpp

add_subdirectory(core)

if(ENABLE_PULL)
  add_subdirectory(pull)
endif()

if(ENABLE_PUSH)
  add_subdirectory(push)
endif()

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
