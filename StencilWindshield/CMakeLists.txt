cmake_minimum_required(VERSION 2.8)

project(StencilWindshield)

add_executable(StencilWindshield main.cpp)
target_link_libraries(StencilWindshield PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( StencilWindshield PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS StencilWindshield DESTINATION ${PG_SOURCE_DIR}/bin)
