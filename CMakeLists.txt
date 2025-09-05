cmake_minimum_required(VERSION 3.20)
project(Memory)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MinGW-w64 cross-compilation settings
if(CMAKE_CROSSCOMPILING)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
    set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
    set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++ -Wl,--kill-at")
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/memory.cpp
    src/imgui_draw.cpp
    src/imgui_tables.cpp
    src/imgui_widgets.cpp
    src/imgui.cpp
)

# Headers - Fixed the imgui.h path
set(HEADERS
    include/Nexus.h
    include/memory.h
    include/memtools.h
    include/imgui.h
    include/imgui_internal.h
    include/imconfig.h
    include/imstb_rectpack.h
    include/imstb_textedit.h
    include/imstb_truetype.h
    include/imgui_memory_editor.h
)

# Include directories
include_directories(include)

# Create the addon DLL
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# Set output name without lib prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Platform-specific settings
if(WIN32 OR CMAKE_CROSSCOMPILING)
    # Windows/Cross-compilation specific settings
    # Use .dll for GW2/Nexus (even when running under Proton)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        SUFFIX ".dll"
        LINK_FLAGS "-Wl,--export-all-symbols"
    )
else()
    # Native Linux build (not for GW2 - this would be for other Linux apps)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        SUFFIX ".so"
    )
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -O2
    )
endif()

# Define version info (optional)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    ADDON_VERSION_MAJOR=0
    ADDON_VERSION_MINOR=0
    ADDON_VERSION_BUILD=1
    ADDON_VERSION_REVISION=0
)