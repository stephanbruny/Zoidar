cmake_minimum_required(VERSION 3.24)
project(Zoidar)

set(CMAKE_CXX_STANDARD 17)

set(RAYLIB_VERSION 4.2.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

add_executable(Zoidar main.cpp src/Sprite.cpp src/Sprite.h src/Textures.cpp src/Textures.h src/Process.cpp src/Process.h lib/Perlin-Noise.hpp src/Tilemap.cpp src/Tilemap.hpp src/WFCMap.hpp)

target_link_libraries(${PROJECT_NAME} raylib)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()