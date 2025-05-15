# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(ModuMIDI C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable
add_executable(ModuMIDI 
    firmware/core/main.c
)

pico_set_program_name(ModuMIDI "ModuMIDI")
pico_set_program_version(ModuMIDI "0.1")

# Configure stdio output
pico_enable_stdio_uart(ModuMIDI 1)
pico_enable_stdio_usb(ModuMIDI 0)

target_sources(ModuMIDI PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/firmware/core/usb_descriptors.c
        ${CMAKE_CURRENT_LIST_DIR}/firmware/core/midi.c
        ${CMAKE_CURRENT_LIST_DIR}/firmware/core/encoder.c
        ${CMAKE_CURRENT_LIST_DIR}/firmware/core/switch.c
        )
        
# Add include directories for main executable
target_include_directories(ModuMIDI PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/firmware/core
    ${CMAKE_CURRENT_LIST_DIR}/firmware/peripherals
)

# Link libraries
target_link_libraries(ModuMIDI 
    pico_stdlib
    hardware_i2c
    tinyusb_device
    tinyusb_board
)

# Create additional output formats
pico_add_extra_outputs(ModuMIDI)

