cmake_minimum_required(VERSION 3.25.1)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ascii_art)
add_subdirectory(lib/ffmpeg)
# find_package(Curses REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp )
add_executable(${PROJECT_NAME} ${SRC_FILES})

link_directories(${PROJECT_NAME} /opt/homebrew/opt/ncurses/include)
link_libraries(${PROJECT_NAME} /opt/homebrew/opt/ncurses/lib )

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} ffmpeg ncurses)
