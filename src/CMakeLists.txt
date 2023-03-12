set(SDL_NYAN_WARN_FLAGS -Wall -Wextra -Wpedantic)

# Source: log.c by rxi (https://github.com/rxi/log.c)
add_library(nyan_logc OBJECT log.c)
target_compile_features(nyan_logc PRIVATE c_std_11)
if (NOT WIN32)
    target_compile_definitions(nyan_logc PRIVATE -DLOG_USE_COLOR)
endif()
target_compile_options(nyan_logc PUBLIC -fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=. PRIVATE ${SDL_NYAN_WARN_FLAGS})

set(CMAKE_C_CLANG_TIDY clang-tidy -p ${CMAKE_BINARY_DIR} --extra-arg=-std=c11)
set(CMAKE_CXX_CLANG_TIDY clang-tidy -p ${CMAKE_BINARY_DIR} --extra-arg=-std=c++17)

add_library(sdl_nyan STATIC sdl_nyan.cc)
target_compile_features(sdl_nyan PRIVATE cxx_std_17)
target_link_libraries(sdl_nyan
    PRIVATE nyan_logc
    PRIVATE SDL2::SDL2
)
target_include_directories(sdl_nyan
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_executable(sdl_nyan_demo sdl_nyan_demo.cc)
target_compile_features(sdl_nyan_demo PRIVATE cxx_std_17)
target_link_libraries(sdl_nyan_demo
    PRIVATE sdl_nyan
    PRIVATE SDL2::SDL2
)

if (WIN32)
    target_link_libraries(sdl_nyan_demo PRIVATE SDL2::SDL2main)
endif()
