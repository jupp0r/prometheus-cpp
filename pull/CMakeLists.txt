if(USE_THIRDPARTY_LIBRARIES)
  find_package(civetweb-3rdparty CONFIG REQUIRED)
  add_library(${PROJECT_NAME}::civetweb ALIAS civetweb)
  install(
    TARGETS civetweb
    EXPORT ${PROJECT_NAME}-targets
  )
  find_package(cppcodec-3rdparty CONFIG REQUIRED)
  add_library(${PROJECT_NAME}::cppcodec ALIAS cppcodec)
  install(
    TARGETS cppcodec
    EXPORT ${PROJECT_NAME}-targets
  )
else()
  find_package(civetweb CONFIG REQUIRED)
  find_package(cppcodec REQUIRED)

  # work-around https://github.com/civetweb/civetweb/pull/918
  if(WIN32 AND NOT TARGET WINSOCK::WINSOCK)
    add_library(WINSOCK::WINSOCK INTERFACE IMPORTED)
    target_link_libraries(WINSOCK::WINSOCK INTERFACE ws2_32)
  endif()
endif()

if(ENABLE_COMPRESSION)
  find_package(ZLIB REQUIRED)
endif()

add_library(pull
  src/basic_auth.cc
  src/basic_auth.h
  src/endpoint.cc
  src/endpoint.h
  src/exposer.cc
  src/handler.cc
  src/handler.h
  src/metrics_collector.cc
  src/metrics_collector.h
)

add_library(${PROJECT_NAME}::pull ALIAS pull)

target_link_libraries(pull
  PUBLIC
    ${PROJECT_NAME}::core
  PRIVATE
    Threads::Threads
    $<IF:$<BOOL:${USE_THIRDPARTY_LIBRARIES}>,${PROJECT_NAME}::civetweb,civetweb::civetweb-cpp>
    $<$<BOOL:${USE_THIRDPARTY_LIBRARIES}>:${PROJECT_NAME}::cppcodec>
    $<$<AND:$<BOOL:UNIX>,$<NOT:$<BOOL:APPLE>>>:rt>
    $<$<BOOL:${ENABLE_COMPRESSION}>:ZLIB::ZLIB>
)

target_include_directories(pull
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  PRIVATE
    ${CPPCODEC_INCLUDE_DIRS} # needed as long as upstream cppcodec installs no config file with imported target
    ${CIVETWEB_INCLUDE_DIRS}
)

target_compile_definitions(pull
  PRIVATE
    $<$<BOOL:${ENABLE_COMPRESSION}>:HAVE_ZLIB>
)

set_target_properties(pull
  PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}-pull
    DEFINE_SYMBOL PROMETHEUS_CPP_PULL_EXPORTS
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

generate_export_header(pull
  BASE_NAME ${PROJECT_NAME}-pull
  EXPORT_FILE_NAME include/prometheus/detail/pull_export.h
)

install(
  TARGETS pull
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
