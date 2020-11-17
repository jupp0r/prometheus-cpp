
add_executable(prometheus_pull_test
  exposer_test.cc
  handler_test.cc
  ${TEST_SRC_FILES}
)

add_compile_definitions(MOCK_MG)

target_link_libraries(prometheus_pull_test
  PRIVATE
    ${PROJECT_NAME}::pull
    GTest::gmock_main
)

target_include_directories(prometheus_pull_test
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/pull/src>
    ${CPPCODEC_INCLUDE_DIRS} # needed as long as upstream cppcodec installs no config file with imported target
    ${CIVETWEB_INCLUDE_DIRS}
)

add_test(
  NAME prometheus_pull_test
  COMMAND prometheus_pull_test
)
