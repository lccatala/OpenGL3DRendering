cmake_minimum_required(VERSION 2.8)

project(BumpMapping)

add_executable(BumpMapping main.cpp)
target_link_libraries(BumpMapping PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( BumpMapping PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS BumpMapping DESTINATION ${PG_SOURCE_DIR}/bin)
