project(reverse_shell_service)

add_compile_definitions(UNICODE)
add_compile_definitions(WIN32_LEAN_AND_MEAN) # exclude rarely used stuff

add_executable(svc main.c)
