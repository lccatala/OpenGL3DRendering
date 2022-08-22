cmake_minimum_required(VERSION 2.8)

project(Lighting)

add_executable(Lighting main.cpp)
target_link_libraries(Lighting PGUPV)

set_target_properties( Lighting PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS Lighting DESTINATION ${PG_SOURCE_DIR}/bin)
