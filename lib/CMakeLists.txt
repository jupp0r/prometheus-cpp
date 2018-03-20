# apply hack to preserve cpp include prefix for generated protobuf files
set(METRICS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/3rdparty/prometheus_client_model)
set(METRICS_SOURCE_FILE ${METRICS_SOURCE_DIR}/metrics.proto)

set(METRICS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cpp)
file(MAKE_DIRECTORY ${METRICS_BINARY_DIR})

add_custom_command(
  OUTPUT "${METRICS_BINARY_DIR}/metrics.pb.cc"
         "${METRICS_BINARY_DIR}/metrics.pb.h"
  COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
  ARGS --cpp_out ${METRICS_BINARY_DIR} -I ${METRICS_SOURCE_DIR} ${METRICS_SOURCE_FILE}
  DEPENDS ${METRICS_PROTO} ${PROTOBUF_PROTOC_EXECUTABLE}
  COMMENT "Running C++ protocol buffer compiler for metrics"
  VERBATIM)

add_library(prometheus-cpp-object OBJECT
  check_names.cc
  counter.cc
  counter_builder.cc
  exposer.cc
  gauge.cc
  gauge_builder.cc
  handler.cc
  handler.h
  histogram.cc
  histogram_builder.cc
  protobuf_delimited_serializer.cc
  registry.cc
  summary.cc
  summary_builder.cc
  text_serializer.cc

  # Metrics Protocol

  ${METRICS_SOURCE_FILE}
  ${METRICS_BINARY_DIR}/metrics.pb.cc
  ${METRICS_BINARY_DIR}/metrics.pb.h
)  


if(DEFINED MAKE_SHARED AND MAKE_SHARED EQUAL 1)
  add_library(prometheus-cpp SHARED
    $<TARGET_OBJECTS:prometheus-cpp-object>

    # civetweb
    $<TARGET_OBJECTS:civetweb>
  )

else()
  add_library(prometheus-cpp
    $<TARGET_OBJECTS:prometheus-cpp-object>

    # civetweb
    $<TARGET_OBJECTS:civetweb>
  )
endif()

target_include_directories(prometheus-cpp-object PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories(prometheus-cpp-object PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(prometheus-cpp-object PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
target_include_directories(prometheus-cpp-object PRIVATE $<BUILD_INTERFACE:${METRICS_BINARY_DIR}>)
target_include_directories(prometheus-cpp-object PRIVATE ${CIVETWEB_INCLUDE_DIR})

# TODO(gj) make all PRIVATE
target_link_libraries(prometheus-cpp PUBLIC ${PROTOBUF_LIBRARIES} ${ZLIB_LIBRARIES} )
target_include_directories(prometheus-cpp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories(prometheus-cpp PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(prometheus-cpp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
target_include_directories(prometheus-cpp PUBLIC $<BUILD_INTERFACE:${METRICS_BINARY_DIR}>)

target_include_directories(prometheus-cpp PRIVATE ${CIVETWEB_INCLUDE_DIR})

install(TARGETS prometheus-cpp EXPORT prometheus-cpp-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  FILES
    ${METRICS_BINARY_DIR}/metrics.pb.h
  DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/prometheus
)

install(DIRECTORY ../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  set(CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
  include(CMakePackageConfigHelpers)
  configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/prometheus-cpp-config.cmake.in prometheus-cpp-config.cmake
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )
  install(EXPORT prometheus-cpp-targets DESTINATION ${CMAKECONFIG_INSTALL_DIR} NAMESPACE prometheus-cpp::)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/prometheus-cpp-config.cmake" DESTINATION ${CMAKECONFIG_INSTALL_DIR})
