function(enable_compile_option target_name)
    if(BUILD_WARNINGS)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            list(APPEND MSVC_OPTIONS
                "/W3"
                "/WX"
                "/permissive-"
                "/W4"
                "/w14242"
                "/w14254"
                "/w14263"
                "/w14265"
                "/w14287"
                "/we4289"
                "/w14296"
                "/w14311"
                "/w14545"
                "/w14546"
                "/w14547"
                "/w14549"
                "/w14555"
                "/w14619"
                "/w14640"
                "/w14826"
                "/w14905"
                "/w14906"
                "/w14928")
        endif()

        set(BASE_OPTIONS
            "-Wall"
            "-Wextra"
            "-Weffc++"
            "-Werror=uninitialized"
            "-Werror=return-type"
            "-Wconversion"
            "-Wsign-compare"
            "-Werror=unused-result"
            "-Werror=suggest-override"
            "-Wzero-as-null-pointer-constant"
            "-Wmissing-declarations"
            "-Wold-style-cast"
            "-Werror=vla"
            "-Wnon-virtual-dtor")
        target_compile_options(${target_name} PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:Clang>,
                   $<CXX_COMPILER_ID:AppleClang>,
                   $<CXX_COMPILER_ID:GNU>>:${BASE_OPTIONS}>
            $<$<CXX_COMPILER_ID:MSVC>:${MSVC_OPTIONS}>)
    endif()
endfunction()

function(enable_addr_sanitizer target_name)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "Sanitizer supported only for gcc/clang")
    endif()
    message(STATUS "Address sanitizer enabled")
    target_compile_options(${target_name} PRIVATE
        -fsanitize=address,undefined
        -fno-sanitize=signed-integer-overflow
        -fno-sanitize-recover=all
        -fno-omit-frame-pointer)
    target_link_libraries(${target_name} PRIVATE -fsanitize=address,undefined )
endfunction()

function(enable_thread_sanitizer target_name)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "Sanitizer supported only for gcc/clang")
    endif()
    message(STATUS "Thread sanitizer enabled")
    target_compile_options(${target_name} PRIVATE
        -fsanitize=thread
        -fno-omit-frame-pointer
        -shared-libasan)
    target_link_libraries(${target_name} PRIVATE -fsanitizer=thread)
endfunction()