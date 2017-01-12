add_executable(sample_server
  sample_server.cc
)

target_link_libraries(sample_server PRIVATE prometheus-cpp)
target_include_directories(sample_server PRIVATE ${PROJECT_SOURCE_DIR}) # fixme

#[[
if(Telegraf_FOUND)
  add_test(NAME sample_server COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scrape.sh)
endif()
]]
