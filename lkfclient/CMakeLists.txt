cmake_minimum_required (VERSION 3.15)

set(lua_target "lkfclient")
set(lua_library_name "kfclient")

find_package(Lua REQUIRED 5.3)

if (APPLE)
    add_library(${lua_target} MODULE lkfclient.cpp)
    set_target_properties(${lua_target} PROPERTIES SUFFIX ".so")
    target_link_options(${lua_target} PUBLIC "SHELL:-undefined dynamic_lookup")
else()
    add_library(${lua_target} SHARED lkfclient.cpp)
    target_link_libraries(${lua_target} PUBLIC ${LUA_LIBRARIES})
endif()

target_include_directories(${lua_target} PUBLIC ${LUA_INCLUDE_DIR})

target_link_libraries(${lua_target} PUBLIC Boost::system)
target_link_libraries(${lua_target} PUBLIC kfclient)

set_target_properties(${lua_target} PROPERTIES PREFIX "")
set_target_properties(${lua_target} PROPERTIES OUTPUT_NAME ${lua_library_name})

install(TARGETS ${lua_target} DESTINATION lib/lua/5.3)

