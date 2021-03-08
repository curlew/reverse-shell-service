project(reverse_shell_service)

add_compile_definitions(UNICODE)
add_compile_definitions(WIN32_LEAN_AND_MEAN) # exclude rarely used stuff

add_executable(svc main.c service.c shell.c)

target_link_libraries(svc Ws2_32)
