
if(USE_THIRDPARTY_LIBRARIES)
  find_package(civetweb-3rdparty CONFIG REQUIRED)
else()
  find_package(CivetWeb REQUIRED)
endif()

if(ENABLE_COMPRESSION)
  find_package(ZLIB REQUIRED)
endif()

set(sources
  src/exposer.cc
  src/handler.cc
  src/handler.h
)

add_library(pull STATIC
  ${sources}
  $<$<BOOL:${USE_THIRDPARTY_LIBRARIES}>:$<TARGET_OBJECTS:civetweb>>
)

add_library(${PROJECT_NAME}::pull ALIAS pull)

target_link_libraries(pull
  PUBLIC
    ${PROJECT_NAME}::core
  PRIVATE
    Threads::Threads
    ${CIVETWEB_LIBRARIES}
    $<$<AND:$<BOOL:UNIX>,$<NOT:$<BOOL:APPLE>>>:rt>
    $<$<BOOL:${ENABLE_COMPRESSION}>:ZLIB::ZLIB>
)

target_include_directories(pull
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CIVETWEB_INCLUDE_DIRS}
)

target_compile_definitions(pull
  PRIVATE
    $<$<BOOL:${ENABLE_COMPRESSION}>:HAVE_ZLIB>
)

set_target_properties(pull PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-pull)

install(
  TARGETS pull
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if (ENABLE_SHARED_BUILD)
add_library(pull_shared SHARED
  ${sources}
  $<$<BOOL:${USE_THIRDPARTY_LIBRARIES}>:$<TARGET_OBJECTS:civetweb>>
)
add_library(${PROJECT_NAME}::pull_shared ALIAS pull_shared)

target_link_libraries(pull_shared
  PUBLIC
    ${PROJECT_NAME}::core
  PRIVATE
    Threads::Threads
    ${CIVETWEB_LIBRARIES}
    $<$<AND:$<BOOL:UNIX>,$<NOT:$<BOOL:APPLE>>>:rt>
    $<$<BOOL:${ENABLE_COMPRESSION}>:ZLIB::ZLIB>
)

target_include_directories(pull_shared
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CIVETWEB_INCLUDE_DIRS}
)

target_compile_definitions(pull_shared
  PRIVATE
    $<$<BOOL:${ENABLE_COMPRESSION}>:HAVE_ZLIB>
)

set_target_properties(pull_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-pull)

install(
  TARGETS pull_shared
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
endif()

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()
