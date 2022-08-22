cmake_minimum_required(VERSION 2.8)

project(p1)

add_executable(p1 main.cpp)
target_link_libraries(p1 PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( p1 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS p1 DESTINATION ${PG_SOURCE_DIR}/bin)
