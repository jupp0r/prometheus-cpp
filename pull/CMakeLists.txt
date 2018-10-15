if(USE_THIRDPARTY_LIBRARIES)
  find_package(civetweb-3rdparty CONFIG REQUIRED PATHS ${PROJECT_SOURCE_DIR}/cmake)
  set(CIVETWEB_SOURCES $<TARGET_OBJECTS:civetweb>)
else()
  find_package(CivetWeb REQUIRED)
endif()

add_library(prometheus-cpp-pull
  src/exposer.cc
  src/handler.cc
  src/handler.h

  ${CIVETWEB_SOURCES}
)

set_target_properties(prometheus-cpp-pull PROPERTIES EXPORT_NAME pull)

target_link_libraries(prometheus-cpp-pull PUBLIC prometheus-cpp-core)
target_link_libraries(prometheus-cpp-pull PRIVATE ${CMAKE_THREAD_LIBS_INIT})
if(UNIX AND NOT APPLE)
  target_link_libraries(prometheus-cpp-pull PRIVATE rt)
endif()

target_include_directories(prometheus-cpp-pull PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_include_directories(prometheus-cpp-pull PRIVATE ${CIVETWEB_INCLUDE_DIRS})
target_link_libraries(prometheus-cpp-pull PRIVATE ${CIVETWEB_LIBRARIES})

if(ENABLE_COMPRESSION)
  find_package(ZLIB REQUIRED)
  target_link_libraries(prometheus-cpp-pull PRIVATE ${ZLIB_LIBRARIES})
  target_include_directories(prometheus-cpp-pull PUBLIC $<BUILD_INTERFACE:${ZLIB_INCLUDE_DIRS}>)
  target_compile_definitions(prometheus-cpp-pull PRIVATE HAVE_ZLIB)
endif()

install(TARGETS prometheus-cpp-pull EXPORT prometheus-cpp-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()
