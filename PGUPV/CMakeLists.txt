cmake_minimum_required(VERSION 3.0)

# PGUPV Library

find_package(OpenGL REQUIRED)

file(GLOB SRCS "*.cpp")

IF (WIN32)

MESSAGE("Windows detected.")
SET(ASSIMP_INCLUDE_DIR ${PG_SOURCE_DIR}/librerias/assimp/include)
SET(FFMPEG_INCLUDE_DIR ${PG_SOURCE_DIR}/librerias/ffmpeg/include)

ELSE(WIN32)
find_package(SDL2 REQUIRED)
find_package(assimp REQUIRED 5)

#FIND_PATH(ASSIMP_INCLUDE_DIR assimp/scene.h)
FIND_PACKAGE(FREEIMAGE)
FIND_PACKAGE(FFMPEG)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
else(NOT OPENGL_FOUND)
	message(STATUS ${OpenGL_INCLUDE_DIRS})
	message(STATUS ${OpenGL_LIBRARY_DIRS})
	message(STATUS ${OpenGL_DEFINITIONS})
endif(NOT OPENGL_FOUND)
SET(EXTRA_LIBS  ${OPENGL_LIBRARIES}  SDL2_ttf ${FFMPEG_LIBRARIES})

ENDIF(WIN32)


add_library(PGUPV ${SRCS} ${PG_SOURCE_DIR}/librerias/glew/src/glew.c)

target_include_directories(PGUPV PUBLIC include/ ${PG_SOURCE_DIR}/librerias/glm)

target_link_libraries(PGUPV PUBLIC ${EXTRA_LIBS} )
target_link_libraries(PGUPV PRIVATE OpenGL::GL SDL2::SDL2 guipg ${ASSIMP_LIBRARIES} ${FREEIMAGE_LIBRARIES})

INCLUDE_DIRECTORIES(
	${PG_SOURCE_DIR}/librerias/boost  
	${PG_SOURCE_DIR}/librerias/glew/include 
	${PG_SOURCE_DIR}/librerias/json 
	${FREEIMAGE_INCLUDE_DIR} 
	${PG_SOURCE_DIR}/librerias/gli 
	${FFMPEG_INCLUDE_DIR} 
	${PG_SOURCE_DIR}/librerias/GSL/include
	)


install(TARGETS PGUPV DESTINATION ${PG_SOURCE_DIR}/lib)

