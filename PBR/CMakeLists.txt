cmake_minimum_required(VERSION 2.8)

project(PBR)

add_executable(PBR main.cpp)
target_link_libraries(PBR PGUPV)

set_target_properties( PBR PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS PBR DESTINATION ${PG_SOURCE_DIR}/bin)
