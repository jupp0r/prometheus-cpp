
find_package(CURL REQUIRED)

if(USE_THIRDPARTY_LIBRARIES)
  find_package(cpr-3rdparty CONFIG REQUIRED PATHS ${PROJECT_SOURCE_DIR}/cmake)
  set(CPR_SOURCES $<TARGET_OBJECTS:cpr>)
else()
  find_package(CPR REQUIRED)
endif()


add_library(push
  src/gateway.cc
  ${CPR_SOURCES}
)

add_library(${PROJECT_NAME}::push ALIAS push)

target_link_libraries(push
  PUBLIC
    ${PROJECT_NAME}::core
  PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    ${CURL_LIBRARIES}
    ${CPR_LIBRARIES}
    $<$<AND:$<BOOL:UNIX>,$<NOT:$<BOOL:APPLE>>>:rt>
)

target_include_directories(push
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CPR_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
)

set_target_properties(push PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-push)

install(
  TARGETS push
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()
