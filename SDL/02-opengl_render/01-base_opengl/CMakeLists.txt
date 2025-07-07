project(02-01-base_opengl)

add_library(${PROJECT_NAME} STATIC app.cpp ../main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE 02-opengl_render)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL_wrapper
    glad)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE Gl)
endif()

enable_compile_option(${PROJECT_NAME})
if(NOT CMAKE_SYSTEM_NAME MATCHES "Windows")
    enable_addr_sanitizer(${PROJECT_NAME})
endif()
