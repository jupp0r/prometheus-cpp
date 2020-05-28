
add_executable(sample_server
  sample_server.cc
)

target_link_libraries(sample_server
  PRIVATE
    ${PROJECT_NAME}::pull
)

add_executable(sample_server_multi
  sample_server_multi.cc
)

target_link_libraries(sample_server_multi
  PRIVATE
    ${PROJECT_NAME}::pull
)
