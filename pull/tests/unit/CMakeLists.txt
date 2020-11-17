
add_executable(prometheus_pull_test
  exposer_test.cc
  handler_test.cc
)

target_link_libraries(prometheus_pull_test
  PRIVATE
    ${PROJECT_NAME}::pull
    GTest::gmock_main
)

target_include_directories(prometheus_pull_test
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/pull/src>
    ${CIVETWEB_INCLUDE_DIRS}
)

add_test(
  NAME prometheus_pull_test
  COMMAND prometheus_pull_test
)
