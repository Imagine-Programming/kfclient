cmake_minimum_required (VERSION 3.15)

project ("kfclient")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.45.0 REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

if (WIN32)
    add_definitions(-DKFCLIENT_WINDOWS)
endif (WIN32)

if (UNIX)
    add_definitions(-DKFCLIENT_UNIX)
endif (UNIX)

option(BUILD_CLI "Build the CLI client" ON)
option(BUILD_LUA "Build the Lua library" ON)

add_subdirectory(kfclient)

if (BUILD_CLI)
    add_subdirectory(kfclient-cli)
endif()

if (BUILD_LUA)
    add_subdirectory(lkfclient)
endif()

