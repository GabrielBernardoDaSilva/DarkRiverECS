cmake_minimum_required(VERSION 3.5)

project(ForgedInLostLandsEcs VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(./include)

add_executable(ForgedInLostLandsEcs ${SOURCES})