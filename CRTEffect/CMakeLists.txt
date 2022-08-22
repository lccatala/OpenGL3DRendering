cmake_minimum_required(VERSION 2.8)

project(CRTEffect)

add_executable(CRTEffect main.cpp)
target_link_libraries(CRTEffect PGUPV)

set_target_properties( CRTEffect PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS CRTEffect DESTINATION ${PG_SOURCE_DIR}/bin)
