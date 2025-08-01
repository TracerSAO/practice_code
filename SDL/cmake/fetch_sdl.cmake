macro(fetch_sdl)
    include(FetchContent)

    FetchContent_Declare(SDL SYSTEM
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG release-3.2.16)

    # 可选：设置SDL的构建选项
    set(SDL_TEST   OFF  CACHE BOOL "" FORCE)
    set(SDL_SHARED ON   CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF  CACHE BOOL "" FORCE)
    set(SDL_ASAN   OFF  CACHE BOOL "" FORCE)
    set(SDL_CCACHE ON   CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(SDL)
endmacro(fetch_sdl)
