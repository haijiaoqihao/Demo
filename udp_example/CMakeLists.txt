cmake_minimum_required(VERSION 3.5)

project(udp_demo)

find_package(yaml-cpp REQUIRED)


add_executable(client udp_client.cc)
target_include_directories(client PUBLIC ${YAML_CPP_INCLUDE_DIR})
target_link_libraries(client yaml-cpp)

add_executable(server udp_server.cc)
target_include_directories(client PUBLIC ${YAML_CPP_INCLUDE_DIR})
target_link_libraries(server yaml-cpp)

