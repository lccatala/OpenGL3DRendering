cmake_minimum_required(VERSION 2.8)

project(MagicOrb)

add_executable(MagicOrb main.cpp)
target_link_libraries(MagicOrb PGUPV)

set_target_properties( MagicOrb PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS MagicOrb DESTINATION ${PG_SOURCE_DIR}/bin)
