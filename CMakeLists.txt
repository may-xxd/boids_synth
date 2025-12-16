cmake_minimum_required(VERSION 3.13)

project(boids-synth C ASM)

set (BUILD_SHARED OFF FORCE)
set (BUILD_STATIC ON FORCE)

set(default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_subdirectory(deps/raylib EXCLUDE_FROM_ALL)

set(BOIDS_SYNTH_SRC
  src/main.c
  )

file(GLOB TRIPOD_SRC deps/tripod/src/tp/*.c deps/tripod/src/tp/*.S)


add_executable(boids-synth ${BOIDS_SYNTH_SRC} ${TRIPOD_SRC})

target_include_directories(boids-synth PRIVATE deps/tripod/src deps/raylib/include)
target_link_libraries(boids-synth PRIVATE m pulse-simple c raylib)

set_property(TARGET boids-synth PROPERTY C_STANDARD 99)

# Enable all warnings
if (MSVC)
  target_compile_options(boids-synth PRIVATE /W4 /WX)
else()
  target_compile_options(boids-synth PRIVATE -Wall -Wextra -pedantic -Werror -Wno-missing-braces -Wno-unused-function -fno-stack-protector)
  target_link_options(boids-synth PRIVATE -nostdlib)
endif()

