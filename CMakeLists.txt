cmake_minimum_required(VERSION 3.30)
project(Renderer3D)

set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS OFF)

# Optimize Assimp build
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)

# Add project files
add_subdirectory(src)

# Add 3rd party libraries
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/stb_image)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/spdlog)

# Copy assets to build directory
add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(Renderer3D copy_assets)

# Disable warning for unknown pragma - MSVC specific code
target_compile_options(assimp PRIVATE -Wno-unknown-pragmas)