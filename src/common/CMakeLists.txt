cmake_minimum_required(VERSION 3.28)

project(
  common
  DESCRIPTION "Common Utilities and Types"
  LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PRIVATE pack.cpp parse.cpp udp_socket.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${TFTP_INCLUDE_DIR})
