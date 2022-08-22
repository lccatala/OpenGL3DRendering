cmake_minimum_required(VERSION 2.8)

project(TessellatedTerrain)

add_executable(TessellatedTerrain main.cpp)
target_link_libraries(TessellatedTerrain PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( TessellatedTerrain PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS TessellatedTerrain DESTINATION ${PG_SOURCE_DIR}/bin)
