cmake_minimum_required(VERSION 3.2)
project(web_server_cpp_blog_proj)


add_executable(web_server_cpp_blog src/main.cpp)
set_target_properties(web_server_cpp_blog PROPERTIES CXX_STANDARD 20)

add_subdirectory(external/web_server_cpp)
target_link_libraries(web_server_cpp_blog PUBLIC networking_lib::networking_lib)

add_subdirectory(external/nlohmann_json)
target_link_libraries(web_server_cpp_blog PUBLIC nlohmann_json::nlohmann_json)

set(INJA_USE_EMBEDDED_JSON FALSE)
add_subdirectory(external/inja)
target_link_libraries(web_server_cpp_blog PUBLIC pantor::inja)