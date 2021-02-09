cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)
set(PIMORONI_SDK D:/Projects/pimoroni-pico/)
include(pimoroni_pico_display_import.cmake)

project(pico_fire C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(pico_fire pico_fire.cpp)

# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(pico_fire pico_stdlib pico_display hardware_spi hardware_pwm)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(pico_fire)
