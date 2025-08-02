project(stb LANGUAGES CXX C)

add_library(${PROJECT_NAME} INTERFACE)
add_library(3rd::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE include)
