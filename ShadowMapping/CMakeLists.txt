cmake_minimum_required(VERSION 2.8)

project(ShadowMapping)

add_executable(ShadowMapping main.cpp)
target_link_libraries(ShadowMapping PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( ShadowMapping PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS ShadowMapping DESTINATION ${PG_SOURCE_DIR}/bin)
