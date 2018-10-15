if(GoogleBenchmark_FOUND)
  add_subdirectory(benchmark)
endif()

add_executable(prometheus_test
  check_names_test.cc
  counter_test.cc
  family_test.cc
  gauge_test.cc
  histogram_test.cc
  mock_metric.h
  registry_test.cc
  summary_test.cc
)

target_link_libraries(prometheus_test PRIVATE prometheus-cpp-core)
target_include_directories(prometheus_test PRIVATE ${PROJECT_SOURCE_DIR}) # fixme

target_link_libraries(prometheus_test PRIVATE GTest::gmock_main)

add_test(NAME prometheus_test COMMAND $<TARGET_FILE:prometheus_test>)
