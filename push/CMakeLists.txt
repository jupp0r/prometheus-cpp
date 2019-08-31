
find_package(CURL REQUIRED)

add_library(push
  src/gateway.cc
)

add_library(${PROJECT_NAME}::push ALIAS push)

generate_export_header(push
  EXPORT_FILE_NAME include/prometheus/detail/push_export.h
  PREFIX_NAME PROMETHEUS_CPP_
)

target_link_libraries(push
  PUBLIC
    ${PROJECT_NAME}::core
  PRIVATE
    Threads::Threads
    ${CURL_LIBRARIES}
    $<$<AND:$<BOOL:UNIX>,$<NOT:$<BOOL:APPLE>>>:rt>
)

target_include_directories(push
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  PRIVATE
    ${CURL_INCLUDE_DIRS}
)

set_target_properties(push
  PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}-push
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

install(
  TARGETS push
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY include/ ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()
