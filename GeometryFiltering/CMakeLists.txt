cmake_minimum_required(VERSION 2.8)

project(GeometryFiltering)

add_executable(GeometryFiltering main.cpp)
target_link_libraries(GeometryFiltering PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( GeometryFiltering PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS GeometryFiltering DESTINATION ${PG_SOURCE_DIR}/bin)
