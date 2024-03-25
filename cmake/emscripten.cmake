#EMSCRIPTEN
option(${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_GEN_HTML "EMSCRIPTEN generateq .wasm and .js file. If you set this option to ON we also generate an html container" OFF)
set(${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_MEMORY 256MB CACHE STRING "Specify emscripten default memory size")
set(${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_STACK 4MB CACHE STRING "Specify emscripten default stack size")
set(${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_JS_EXCEPTION OFF CACHE BOOL "Specify if we enable exceptions via Emscripten’s JavaScript-based support.")
set(${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_WASM_EXCEPTION OFF CACHE BOOL "Specify if we enable exceptions via Emscripten’s WebAssembly-based support.")
set(${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_THREAD_POOL 4 CACHE STRING "Specify emscripten default thread pool size")
set(${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_FS_SRC_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../assets CACHE STRING "Specify emscripten preload file dir")
set(${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_FS_DST_ROOT_DIR assets CACHE STRING "Specify emscripten fs destination dir")

target_include_directories(${PROJECT_NAME_LOWER_MINUS} 
PUBLIC
    $<BUILD_INTERFACE:${EMSCRIPTEN_SYSROOT}>
)
target_link_libraries(${PROJECT_NAME_LOWER_MINUS} 
PRIVATE
#      "-lwebsocket.js"
)

message("emscripten default memory size: ${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_MEMORY}")
message("emscripten default stack size: ${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_STACK}")
message("emscripten default thread pool size: ${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_THREAD_POOL}")
message("emscripten fs source file directory: ${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_FS_SRC_ROOT_DIR}")
message("emscripten fs destination file directory: ${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_FS_DST_ROOT_DIR}") 
message("emscripten use exceptions via JavaScript ${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_JS_EXCEPTION}") 
message("emscripten use exceptions via WebAssembly ${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_WASM_EXCEPTION}") 

#https://decovar.dev/blog/2023/11/20/webassembly-with-pthreads/
set(HELLOIMGUI_EMSCRIPTEN_PTHREAD ON)
set(HELLOIMGUI_EMSCRIPTEN_PTHREAD_ALLOW_MEMORY_GROWTH OFF)

if (${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_GEN_HTML)
message("===EMSCRIPTEN=== Add .html to .wasm and .js files")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")   #to generate .html in addition to .js and .wasm
else()
	message("===EMSCRIPTEN=== Generate .wasm and .js files")
endif()

string(FIND "${CMAKE_CXX_FLAGS}" "-pthread" FLG_POS)
if(FLG_POS EQUAL -1)
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -pthread")  #hang with imgui: -s NO_DISABLE_EXCEPTION_CATCHING
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")  #hang with imgui: -s NO_DISABLE_EXCEPTION_CATCHING
endif()

if (${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_JS_EXCEPTION)
	string(FIND "${CMAKE_CXX_FLAGS}" "-fexceptions" FLG_POS)
	if(FLG_POS EQUAL -1)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fexceptions")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
	endif()
endif()
	
if (${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_WASM_EXCEPTION)
	string(FIND "${CMAKE_CXX_FLAGS}" "-fwasm-exceptions" FLG_POS)
	if(FLG_POS EQUAL -1)
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fwasm-exceptions")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwasm-exceptions")
	endif()		
endif()

#The @ symbol is needed because sometimes it is useful to package files that are not nested below the compile-time directory, and for which #there is therefore no default mapping to a location in the virtual file system.
string(FIND "${CMAKE_EXE_LINKER_FLAGS}" "-s TOTAL_STACK=" FLG_POS)
if(FLG_POS EQUAL -1)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s TOTAL_STACK=${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_STACK} -s TOTAL_MEMORY=${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_MEMORY} -s PTHREAD_POOL_SIZE=${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_THREAD_POOL} --embed-file ${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_FS_SRC_ROOT_DIR}@${${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_FS_DST_ROOT_DIR}")    
endif()

if (${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_JS_EXCEPTION)
	string(FIND "${CMAKE_EXE_LINKER_FLAGS}" "--fexceptions" FLG_POS)
	if(FLG_POS EQUAL -1)
		set(CMAKE_EXE_LINKER_FLAGS   "${CMAKE_EXE_LINKER_FLAGS}   -fexceptions")  
	endif()
endif()

if (${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_WASM_EXCEPTION)
	string(FIND "${CMAKE_EXE_LINKER_FLAGS}" "-fwasm-exceptions" FLG_POS)
	if(FLG_POS EQUAL -1)
		set(CMAKE_EXE_LINKER_FLAGS   "${CMAKE_EXE_LINKER_FLAGS}   -fwasm-exceptions")  
	endif()
endif()
message("===EMSCRIPTEN=== CMAKE_EXE_LINKER_FLAGS is ${CMAKE_EXE_LINKER_FLAGS}")

message("===EMSCRIPTEN=== CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")
