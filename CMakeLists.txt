cmake_minimum_required(VERSION 3.28)
project(Sound-Dungeon LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default Build Type
set(DEFAULT_BUILD_TYPE "Debug")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Platform-specific flags
if(WIN32)
    # Windows-specific configurations if needed
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-march=native -Ofast")
endif()

# Set Output Directory (Everything goes into "Sound Dungeon")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Sound_Dungeon/bin)

# Create Directories for Sound Dungeon
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/Sound_Dungeon/include)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/Sound_Dungeon/src)

# Create Directories for WFC
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/WFC/include)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/WFC/src)

# Include SFML
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Define Sound Dungeon Executable (Only Source Files, No Headers!)
add_executable(sound_dungeon 
    ${CMAKE_SOURCE_DIR}/Sound_Dungeon/src/mainSFML.cpp
    ${CMAKE_SOURCE_DIR}/Sound_Dungeon/src/dungeon.cpp
    ${CMAKE_SOURCE_DIR}/Sound_Dungeon/src/room.cpp
    ${CMAKE_SOURCE_DIR}/Sound_Dungeon/src/player.cpp
    ${CMAKE_SOURCE_DIR}/Sound_Dungeon/src/game.cpp
    ${CMAKE_SOURCE_DIR}/Sound_Dungeon/src/pathfinding.cpp
)

# WFC Project (Static Library)
add_library(wfc STATIC ${CMAKE_SOURCE_DIR}/WFC/src/wfc_main.cpp)

# Include Directories
target_include_directories(wfc PUBLIC 
    ${CMAKE_SOURCE_DIR}/WFC/include
)

target_include_directories(sound_dungeon PUBLIC 
	${CMAKE_SOURCE_DIR}/Sound_Dungeon/include/dungeon.hpp
	${CMAKE_SOURCE_DIR}/Sound_Dungeon/include/game.hpp
	${CMAKE_SOURCE_DIR}/Sound_Dungeon/include/pathfinding.hpp
	${CMAKE_SOURCE_DIR}/Sound_Dungeon/include/player.hpp
	${CMAKE_SOURCE_DIR}/Sound_Dungeon/include/room.hpp
)

# Link Libraries
target_link_libraries(sound_dungeon PRIVATE SFML::Graphics SFML::Audio)
target_link_libraries(sound_dungeon PRIVATE wfc)

# Ensure Proper Build Order
add_dependencies(sound_dungeon wfc)

# Post-Build Command: Create Results Directory
add_custom_command(
    TARGET sound_dungeon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_BINARY_DIR}/results
)