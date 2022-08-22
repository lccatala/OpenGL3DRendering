cmake_minimum_required(VERSION 2.8)

project(p2)

add_executable(p2 main.cpp)
target_link_libraries(p2 PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( p2 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS p2 DESTINATION ${PG_SOURCE_DIR}/bin)
