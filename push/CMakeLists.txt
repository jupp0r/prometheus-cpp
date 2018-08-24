find_package(CURL REQUIRED)
find_package(cpr CONFIG REQUIRED PATHS ${PROJECT_SOURCE_DIR}/cmake)

add_library(prometheus-cpp-push
  src/gateway.cc

  # cpr

  $<TARGET_OBJECTS:cpr>
)

set_target_properties(prometheus-cpp-push PROPERTIES EXPORT_NAME push)

target_link_libraries(prometheus-cpp-push PUBLIC prometheus-cpp-core)
target_link_libraries(prometheus-cpp-push PRIVATE ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(prometheus-cpp-push PRIVATE ${CURL_LIBRARIES})
if(UNIX AND NOT APPLE)
  target_link_libraries(prometheus-cpp-push PRIVATE rt)
endif()

target_include_directories(prometheus-cpp-push PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(prometheus-cpp-push PRIVATE ${CPR_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})

install(TARGETS prometheus-cpp-push EXPORT prometheus-cpp-targets
  RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif()
