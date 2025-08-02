project(pratice_opengl)

add_library(${PROJECT_NAME} STATIC main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL_wrapper)

add_subdirectory(01-base_opengl)
add_subdirectory(02-base_shader)
add_subdirectory(03-base_texture)
add_subdirectory(04-base-coordinate_system)