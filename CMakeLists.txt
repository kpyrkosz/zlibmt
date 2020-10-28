cmake_minimum_required(VERSION 3.11)

project(zlibmt)

set(CMAKE_CXX_STANDARD 17)
find_package(ZLIB REQUIRED)

set(SOURCES
  src/director.cpp
  src/worker.cpp
  src/uncompress.cpp
)

set(INCLUDES
  inc/director.hpp
  inc/worker.hpp
  inc/uncompress.hpp
  inc/work.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
target_include_directories(${PROJECT_NAME} PRIVATE inc)
