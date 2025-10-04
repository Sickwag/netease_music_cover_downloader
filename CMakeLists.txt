cmake_minimum_required(VERSION 3.20)
project(netease_music_cover)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable C++ exceptions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

# Find required packages
find_package(Boost REQUIRED COMPONENTS system)

# Include directories
include_directories(${Boost_INCLUDE_DIRS})
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/cover_downloader.cpp
    src/utils.cpp
)

# Create main executable
add_executable(netease_music_cover ${SOURCES})

# Link libraries
target_link_libraries(netease_music_cover
    ${Boost_LIBRARIES}
    ws2_32
    crypt32
)

# Create debug test executable
add_executable(debug_test debug_test.cpp src/cover_downloader.cpp src/utils.cpp)
target_link_libraries(debug_test ${Boost_LIBRARIES} ws2_32 crypt32)

# Create filename test executable
add_executable(test_filename test_filename.cpp)
target_link_libraries(test_filename ws2_32)

# Compiler definitions
target_compile_definitions(netease_music_cover PRIVATE _WIN32_WINNT=0x0601)
target_compile_definitions(debug_test PRIVATE _WIN32_WINNT=0x0601)
target_compile_definitions(test_filename PRIVATE _WIN32_WINNT=0x0601)
