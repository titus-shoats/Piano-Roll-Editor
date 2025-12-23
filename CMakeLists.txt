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

# VSTGUI library source files
set(VSTGUI_LIB_SOURCES
    ${VSTGUI_PATH}/vstgui/lib/animation/animations.cpp
    ${VSTGUI_PATH}/vstgui/lib/animation/animator.cpp
    ${VSTGUI_PATH}/vstgui/lib/animation/timingfunctions.cpp
    ${VSTGUI_PATH}/vstgui/lib/cbitmap.cpp
    ${VSTGUI_PATH}/vstgui/lib/cbitmapfilter.cpp
    ${VSTGUI_PATH}/vstgui/lib/ccolor.cpp
    ${VSTGUI_PATH}/vstgui/lib/cdatabrowser.cpp
    ${VSTGUI_PATH}/vstgui/lib/cdrawcontext.cpp
    ${VSTGUI_PATH}/vstgui/lib/cdrawmethods.cpp
    ${VSTGUI_PATH}/vstgui/lib/cdropsource.cpp
    ${VSTGUI_PATH}/vstgui/lib/cfileselector.cpp
    ${VSTGUI_PATH}/vstgui/lib/cfont.cpp
    ${VSTGUI_PATH}/vstgui/lib/cframe.cpp
    ${VSTGUI_PATH}/vstgui/lib/cgradient.cpp
    ${VSTGUI_PATH}/vstgui/lib/cgraphicspath.cpp
    ${VSTGUI_PATH}/vstgui/lib/clayeredviewcontainer.cpp
    ${VSTGUI_PATH}/vstgui/lib/clinestyle.cpp
    ${VSTGUI_PATH}/vstgui/lib/coffscreencontext.cpp
    ${VSTGUI_PATH}/vstgui/lib/copenglview.cpp
    ${VSTGUI_PATH}/vstgui/lib/cpoint.cpp
    ${VSTGUI_PATH}/vstgui/lib/crect.cpp
    ${VSTGUI_PATH}/vstgui/lib/crowcolumnview.cpp
    ${VSTGUI_PATH}/vstgui/lib/cscrollview.cpp
    ${VSTGUI_PATH}/vstgui/lib/csearchtextedit.cpp
    ${VSTGUI_PATH}/vstgui/lib/csegmentbutton.cpp
    ${VSTGUI_PATH}/vstgui/lib/cshadowviewcontainer.cpp
    ${VSTGUI_PATH}/vstgui/lib/cslider.cpp
    ${VSTGUI_PATH}/vstgui/lib/csplitview.cpp
    ${VSTGUI_PATH}/vstgui/lib/cstring.cpp
    ${VSTGUI_PATH}/vstgui/lib/ctabview.cpp
    ${VSTGUI_PATH}/vstgui/lib/ctextlabel.cpp
    ${VSTGUI_PATH}/vstgui/lib/ctooltipsupport.cpp
    ${VSTGUI_PATH}/vstgui/lib/cview.cpp
    ${VSTGUI_PATH}/vstgui/lib/cviewcontainer.cpp
    ${VSTGUI_PATH}/vstgui/lib/cvstguitimer.cpp
    ${VSTGUI_PATH}/vstgui/lib/events.cpp
    ${VSTGUI_PATH}/vstgui/lib/platform/common/genericoptionmenu.cpp
    ${VSTGUI_PATH}/vstgui/lib/platform/common/generictextedit.cpp
)

# Platform-specific VSTGUI sources
if(WIN32)
    list(APPEND VSTGUI_LIB_SOURCES
        ${VSTGUI_PATH}/vstgui/lib/platform/win32/win32support.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/win32/win32frame.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/win32/win32resourcestream.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/win32/winfileselector.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/win32/winstring.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/win32/wintimer.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/win32/direct2d/d2dbitmap.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/win32/direct2d/d2ddrawcontext.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/win32/direct2d/d2dfont.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/win32/direct2d/d2dgraphicspath.cpp
    )
elseif(APPLE)
    list(APPEND VSTGUI_LIB_SOURCES
        ${VSTGUI_PATH}/vstgui/lib/platform/mac/macstring.mm
        ${VSTGUI_PATH}/vstgui/lib/platform/mac/macfileselector.mm
        ${VSTGUI_PATH}/vstgui/lib/platform/mac/mactimer.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/mac/macclipboard.mm
        ${VSTGUI_PATH}/vstgui/lib/platform/mac/macglobals.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/mac/cfontmac.mm
        ${VSTGUI_PATH}/vstgui/lib/platform/mac/cgdrawcontext.mm
        ${VSTGUI_PATH}/vstgui/lib/platform/mac/macframe.mm
    )
else()
    list(APPEND VSTGUI_LIB_SOURCES
        ${VSTGUI_PATH}/vstgui/lib/platform/linux/x11frame.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/linux/x11platform.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/linux/cairocontext.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/linux/cairofont.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/linux/cairogradient.cpp
        ${VSTGUI_PATH}/vstgui/lib/platform/linux/cairopath.cpp
    )
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
add_executable(PianoRollEditor_VSTGUI ${VSTGUI_SOURCES} ${VSTGUI_LIB_SOURCES})

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
        d2d1
        dwrite
        windowscodecs
        shlwapi
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
