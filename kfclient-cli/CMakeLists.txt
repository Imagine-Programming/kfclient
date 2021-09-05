cmake_minimum_required (VERSION 3.15)

set(cli_target "kfclient-cli")
set(cli_executable_name "kfclient")

add_executable(${cli_target} dynacli.hpp definition.hpp kfclient-cli.cpp)

target_link_libraries(${cli_target} PRIVATE Boost::system)
target_link_libraries(${cli_target} PRIVATE kfclient)

# find and add libfmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${cli_target} PRIVATE fmt::fmt)

# find and add CLI11
find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(${cli_target} PRIVATE CLI11::CLI11)

# fubd abd add libfort
find_package(libfort)
target_link_libraries(${cli_target} PRIVATE libfort::fort)

set_target_properties(${cli_target} PROPERTIES OUTPUT_NAME ${cli_executable_name})

install(TARGETS ${cli_target} DESTINATION bin)