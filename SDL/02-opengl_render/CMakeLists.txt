project(02-opengl_render)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

add_subdirectory(01-base_opengl)