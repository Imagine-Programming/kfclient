cmake_minimum_required (VERSION 3.15)

set(lua_target "lkfclient")
set(lua_library_name "kfclient")

add_library(${lua_target} SHARED lkfclient.cpp)

find_package(Lua REQUIRED 5.3)

# target_link_libraries(${lua_target} PUBLIC Lua)
target_link_libraries(${lua_target} PUBLIC Boost::system)
target_link_libraries(${lua_target} PUBLIC kfclient)

set_target_properties(${lua_target} PROPERTIES PREFIX "")
set_target_properties(${lua_target} PROPERTIES OUTPUT_NAME ${lua_library_name})

install(TARGETS ${lua_target} DESTINATION lib/lua/5.3)