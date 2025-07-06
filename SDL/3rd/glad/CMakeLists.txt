project(glad LANGUAGES C)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS include/*.h src/*.c)

add_library(${PROJECT_NAME} STATIC ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
