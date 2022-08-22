cmake_minimum_required(VERSION 2.8)

project(p0)

add_executable(p0 main.cpp)
target_link_libraries(p0 PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( p0 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS p0 DESTINATION ${PG_SOURCE_DIR}/bin)
