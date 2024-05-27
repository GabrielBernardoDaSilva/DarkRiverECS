cmake_minimum_required(VERSION 3.5)

project(WinterRainECS VERSION 0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(./include)

# if you want to build executable 

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

# add_executable(${PROJECT_NAME} ${SOURCES})