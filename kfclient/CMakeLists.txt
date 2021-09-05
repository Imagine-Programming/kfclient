cmake_minimum_required (VERSION 3.15)

set(library_target "kfclient")

add_library(${library_target} SHARED kfbuffer.hpp kfdetails.hpp kfdetails.cpp kfrules.hpp kfrules.cpp kfplayers.hpp kfplayers.cpp kfclient.hpp kfclient.cpp)

target_link_libraries(${library_target} PUBLIC Threads::Threads)
target_include_directories(${library_target} PUBLIC .)
target_link_libraries(${library_target} PUBLIC Boost::system)
target_compile_definitions(${library_target} PUBLIC -DKFCLIENT_BUILD_DLL)

install(TARGETS ${library_target} DESTINATION lib)