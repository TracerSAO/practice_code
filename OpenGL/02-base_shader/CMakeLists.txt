project(02-base_shader)

add_executable(${PROJECT_NAME} app.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pratice_opengl
    SDL_wrapper
    opengl_wrapper)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE Gl)
endif()

enable_compile_option(${PROJECT_NAME})
if(NOT CMAKE_SYSTEM_NAME MATCHES "Windows")
    enable_addr_sanitizer(${PROJECT_NAME})
endif()
