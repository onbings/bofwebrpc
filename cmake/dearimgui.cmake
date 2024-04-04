string(FIND "${CMAKE_CXX_FLAGS}" "-DHELLOIMGUI_USE_SDL2" FLG_POS)
if(FLG_POS EQUAL -1)
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -DHELLOIMGUI_USE_SDL2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHELLOIMGUI_USE_SDL2")
endif()

string(FIND "${CMAKE_CXX_FLAGS}" "-DHELLOIMGUI_HAS_OPENGL3" FLG_POS)
if(FLG_POS EQUAL -1)
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -DHELLOIMGUI_HAS_OPENGL3")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHELLOIMGUI_HAS_OPENGL3")
endif()
 
target_include_directories(${PROJECT_NAME_LOWER_MINUS}
  PUBLIC
    $<BUILD_INTERFACE:${SDL2_INCLUDE_DIR}>
)

#target_link_libraries(${PROJECT_NAME_LOWER_MINUS}
#  PRIVATE
#	# Note the subtle difference between the package name and the target name: hello-imgui vs hello_imgui!
#    hello-imgui::hello_imgui
#    # this mode will ignore all of hello_imgui cmake tooling, and will not deploy the assets
#
#    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
#    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
#)