cmake_minimum_required(VERSION 3.16)
project(DS_RPG VERSION 0.1 LANGUAGES CXX)

# ─── C++ Standard ──────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Warnings ───────────────────────────────────────────────────────────────────
if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wshadow -Wpedantic -Wunreachable-code)
endif()

# ─── Vcpkg Toolchain (optional if not passed via CLI) ──────────────────────────
if(DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
endif()

# ─── SDL2 Packages ─────────────────────────────────────────────────────────────
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

# ─── Executable ────────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} main.cpp)

# ─── Link Libraries ────────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)
