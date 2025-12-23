cmake_minimum_required(VERSION 3.15)
project(PianoRollEditor_VSTGUI VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# VSTGUI dependency
# You need to have VSTGUI installed or cloned in a known location
# Set VSTGUI_PATH to point to your VSTGUI installation
if(NOT DEFINED VSTGUI_PATH)
    set(VSTGUI_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../vstgui" CACHE PATH "Path to VSTGUI")
endif()

# Check if VSTGUI exists
if(EXISTS "${VSTGUI_PATH}/vstgui/vstgui.h")
    message(STATUS "Found VSTGUI at: ${VSTGUI_PATH}")
else()
    message(WARNING "VSTGUI not found at: ${VSTGUI_PATH}")
    message(WARNING "Please set VSTGUI_PATH to your VSTGUI installation")
    message(WARNING "You can download VSTGUI from: https://github.com/steinbergmedia/vstgui")
endif()

# Source files for VSTGUI version
set(VSTGUI_SOURCES
    Source/VSTGUIMain.cpp
    Source/VSTGUIPianoRollEditorView.cpp
    Source/VSTGUINoteGridView.cpp
    Source/VSTGUIKeyboardView.cpp
    Source/VSTGUITimelineView.cpp
    Source/VSTGUINoteView.cpp
    Source/VSTGUIControlPanelView.cpp
    Source/VSTGUINoteModel.cpp
    Source/VSTGUINoteGridStyleSheet.cpp
)

# Create executable
add_executable(PianoRollEditor_VSTGUI ${VSTGUI_SOURCES})

# Include directories
target_include_directories(PianoRollEditor_VSTGUI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${VSTGUI_PATH}
)

# Compiler definitions
target_compile_definitions(PianoRollEditor_VSTGUI PRIVATE
    LIB_VERSION=1
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(PianoRollEditor_VSTGUI PRIVATE WINDOWS=1)
    target_link_libraries(PianoRollEditor_VSTGUI PRIVATE
        gdi32
        user32
        shell32
        ole32
        uuid
    )
elseif(APPLE)
    target_compile_definitions(PianoRollEditor_VSTGUI PRIVATE MAC=1)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(CARBON_LIBRARY Carbon)
    find_library(ACCELERATE_LIBRARY Accelerate)
    target_link_libraries(PianoRollEditor_VSTGUI PRIVATE
        ${COCOA_LIBRARY}
        ${CARBON_LIBRARY}
        ${ACCELERATE_LIBRARY}
    )
else()
    # Linux
    find_package(X11 REQUIRED)
    find_package(Freetype REQUIRED)
    target_include_directories(PianoRollEditor_VSTGUI PRIVATE
        ${X11_INCLUDE_DIR}
        ${FREETYPE_INCLUDE_DIRS}
    )
    target_link_libraries(PianoRollEditor_VSTGUI PRIVATE
        ${X11_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        pthread
    )
endif()

# VSTGUI library
# Note: VSTGUI is typically header-only or you need to build it separately
# For a complete build, you would add VSTGUI as a subdirectory or link to it
# This is a simplified version - adjust based on your VSTGUI setup

# Install target
install(TARGETS PianoRollEditor_VSTGUI
    RUNTIME DESTINATION bin
)

# Print build info
message(STATUS "Building Piano Roll Editor - VSTGUI Version")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
