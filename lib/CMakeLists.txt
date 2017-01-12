# apply hack to preserve cpp include prefix for generated protobuf files
set(METRICS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/3rdparty/prometheus_client_model)
set(METRICS_SOURCE_FILE ${METRICS_SOURCE_DIR}/metrics.proto)

set(METRICS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cpp)
file(MAKE_DIRECTORY ${METRICS_BINARY_DIR})

add_custom_command(
  OUTPUT "${METRICS_BINARY_DIR}/metrics.pb.cc"
         "${METRICS_BINARY_DIR}/metrics.pb.h"
  COMMAND  ${Protobuf_PROTOC_EXECUTABLE}
  ARGS --cpp_out ${METRICS_BINARY_DIR} -I ${METRICS_SOURCE_DIR} ${METRICS_SOURCE_FILE}
  DEPENDS ${METRICS_PROTO} ${Protobuf_PROTOC_EXECUTABLE}
  COMMENT "Running C++ protocol buffer compiler for metrics"
  VERBATIM)

add_library(prometheus-cpp
  collectable.h
  counter.cc
  counter.h
  counter_builder.cc
  counter_builder.h
  exposer.cc
  exposer.h
  family.h
  gauge.cc
  gauge.h
  gauge_builder.cc
  gauge_builder.h
  handler.cc
  handler.h
  histogram.cc
  histogram.h
  histogram_builder.cc
  histogram_builder.h
  json_serializer.cc
  json_serializer.h
  metric.h
  protobuf_delimited_serializer.cc
  protobuf_delimited_serializer.h
  registry.cc
  registry.h
  serializer.h
  text_serializer.cc
  text_serializer.h

  # Metrics Protocol

  ${METRICS_SOURCE_FILE}
  ${METRICS_BINARY_DIR}/metrics.pb.cc
  ${METRICS_BINARY_DIR}/metrics.pb.h
)

# TODO(gj) make both PRIVATE
target_link_libraries(prometheus-cpp PUBLIC ${Protobuf_LIBRARIES})
target_include_directories(prometheus-cpp PUBLIC ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

# TODO(gj) make PRIVATE
target_link_libraries(prometheus-cpp PUBLIC civetweb)
