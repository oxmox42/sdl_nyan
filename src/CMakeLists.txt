set(SDL_NYAN_WARN_FLAGS -Wall -Wextra -Wpedantic)

# Source: log.c by rxi (https://github.com/rxi/log.c)
add_library(logc OBJECT log.c)
target_compile_features(logc PRIVATE c_std_11)
if (NOT WIN32)
    target_compile_definitions(logc PRIVATE -DLOG_USE_COLOR)
endif()
target_compile_options(logc PUBLIC -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=. PRIVATE ${SDL_NYAN_WARN_FLAGS})

#find_package(Threads)

set(CMAKE_C_CLANG_TIDY clang-tidy -p ${CMAKE_BINARY_DIR} --extra-arg=-std=c11)
set(CMAKE_CXX_CLANG_TIDY clang-tidy -p ${CMAKE_BINARY_DIR} --extra-arg=-std=c++17)

add_executable(sdl_nyan sdl_nyan.cc)
target_compile_features(sdl_nyan PRIVATE cxx_std_17)
target_link_libraries(sdl_nyan
    PRIVATE logc
    PRIVATE SDL2::SDL2-static
)

if (WIN32)
    target_link_libraries(sdl_nyan PRIVATE SDL2::SDL2main)
endif()
