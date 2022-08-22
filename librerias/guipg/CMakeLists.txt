find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_library (guipg 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/imgui.cpp 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/imgui_draw.cpp 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/imgui_tables.cpp 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/imgui_widgets.cpp 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/backends/imgui_impl_sdl.cpp 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/backends/imgui_impl_opengl3.cpp
    nfd_common.c 
    nfd_gtk.c
    guipg.cpp
    icons.cpp
    imGuIZMOquat.cpp
    logConsole.cpp
    )

target_compile_definitions(guipg PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW GLM_ENABLE_EXPERIMENTAL IMGUI_DISABLE_OBSOLETE_FUNCTIONS IMGUIZMO_IMGUI_FOLDER=)

target_include_directories(guipg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(guipg PRIVATE 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui 
    ${CMAKE_SOURCE_DIR}/librerias/dearimgui/backends/ 
    ${GTK3_INCLUDE_DIRS} 
    ${PG_SOURCE_DIR}/librerias/glm
    ${PG_SOURCE_DIR}/librerias/GSL/include
    )
target_link_libraries(guipg PRIVATE ${GTK3_LIBRARIES} OpenGL::GL SDL2::SDL2)




