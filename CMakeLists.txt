cmake_minimum_required (VERSION 3.8)
set(CMAKE_SYSTEM_NAME Generic)
include(C:/VSARM/sdk/pico-sdk/external/pico_sdk_import.cmake)

add_executable(speakerparty main.cpp)

# pull in common dependencies
target_link_libraries(speakerparty pico_stdlib hardware_pwm hardware_i2c)

project(speakerparty)

set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
pico_add_extra_outputs(speakerparty)
