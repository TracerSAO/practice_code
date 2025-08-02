project(glad LANGUAGES C)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS ./include/*.h ./src/*.c)

add_library(${PROJECT_NAME} STATIC ${SRCS})
add_library(3rd::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC include)
