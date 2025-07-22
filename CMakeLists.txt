cmake_minimum_required(VERSION 3.16)
project(DS_RPG VERSION 0.1 LANGUAGES C CXX)

# Set vcpkg toolchain path (only needed if you’re NOT passing it in CLI)
set(CMAKE_TOOLCHAIN_FILE "/mnt/c/Users/nkouk/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/build/vcpkg_installed/x64-linux/share")
set(nlohmann_json_DIR "${CMAKE_PREFIX_PATH}/nlohmann_json")

# ─── C++ Standard ──────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Warnings ───────────────────────────────────────────────────────────────────
if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wshadow -Wpedantic -Wunreachable-code)
endif()

# ─── Dependencies ──────────────────────────────────────────────────────────────
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# ─── Executable ────────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME}
    main.cpp
    hash_table.c
    weapon_loader.cpp
)

# ─── Link Libraries ────────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
        nlohmann_json::nlohmann_json
)