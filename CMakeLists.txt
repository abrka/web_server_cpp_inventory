cmake_minimum_required(VERSION 3.2)
project(web_server_cpp_inventory_proj)


add_executable(web_server_cpp_inventory src/main.cpp)
set_target_properties(web_server_cpp_inventory PROPERTIES CXX_STANDARD 20)

add_subdirectory(external/web_server_cpp)
target_link_libraries(web_server_cpp_inventory PUBLIC networking_lib::networking_lib)


set(BUILD_TEST OFF)
add_subdirectory(external/libpqxx)
target_link_libraries(web_server_cpp_inventory PUBLIC libpqxx::pqxx)

set(BUILD_DOCS OFF)
add_subdirectory(external/dotenv)
target_link_libraries(web_server_cpp_inventory PUBLIC dotenv)

set(BUILD_TESTING OFF)
add_subdirectory(external/nlohmann_json)
target_link_libraries(web_server_cpp_inventory PUBLIC nlohmann_json::nlohmann_json)

set(BUILD_TESTING OFF)
set(INJA_USE_EMBEDDED_JSON FALSE)
add_subdirectory(external/inja)
target_link_libraries(web_server_cpp_inventory PUBLIC pantor::inja)
