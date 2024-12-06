cmake_minimum_required(VERSION 3.29)
project(GameOfLife_GroupA)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get SFML
set(SFML_DIR "C:/Users/matth/CLionProjects/GameOfLife-GroupA/lib/SFML")
include_directories(${SFML_DIR}/include)
link_directories(${SFML_DIR}/lib)
add_definitions(-DSFML_STATIC)

# Collect source files
file(GLOB_RECURSE GAME_OF_LIFE_SOURCES ${CMAKE_SOURCE_DIR}/lib/GameOfLife/*.cpp)

# Include the GameOfLife headers
include_directories(${CMAKE_SOURCE_DIR}/lib/GameOfLife)

# Print source files (for debugging)
message(STATUS "GameOfLife Sources: ${GAME_OF_LIFE_SOURCES}")

# Create a static or shared library from GameOfLife sources
add_library(GameOfLifeLib ${GAME_OF_LIFE_SOURCES})

# Add the main executable
add_executable(GameOfLife main.cpp)

# Link the library to the executable
target_link_libraries(GameOfLife PRIVATE GameOfLifeLib)
target_link_libraries(GameOfLife PRIVATE sfml-graphics sfml-system sfml-window)

# Define DEBUG macro for debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")


# Copy the patterns folder to the build directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Patterns)
file(COPY ${CMAKE_SOURCE_DIR}/lib/Patterns DESTINATION ${CMAKE_BINARY_DIR})
