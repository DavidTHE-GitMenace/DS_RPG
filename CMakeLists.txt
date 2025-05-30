cmake_minimum_required(VERSION 3.16)
project(DS_RPG VERSION 0.1 LANGUAGES CXX)

# ─── Common C++ settings ───────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD      17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Compiler-specific warning flags ───────────────────────────────────────────
if (MSVC)
  # Use level 4 warnings and treat warnings as errors if you like
  add_compile_options(/W4)
else()
  # GCC/Clang flags
  add_compile_options(-Wall -Wextra -Wshadow -Wpedantic -Wunreachable-code)
endif()

# ─── vcpkg & SDL2 setup ────────────────────────────────────────────────────────
find_package(SDL2       CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf   CONFIG REQUIRED)

# ─── Define your executable ────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
)
