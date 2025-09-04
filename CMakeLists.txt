#
# Copyright (c) 2024,2044 OnBings. All rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
# PURPOSE.
#
# SUPER BUILD FOR XTS PROJECT
# 
# History:		1 Jan 2024: BHA: Initial version 
# 

#Emscripten build
#cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_INSTALL_PREFIX=C:/bld/github-web/binaries -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/pro/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_TOOLCHAIN_FILE=C:/pro/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=wasm32-emscripten -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug -SC:/pro/github/bofwebrpc -BC:/bld/bofwebrpc-web -DCMAKE_CROSSCOMPILING_EMULATOR=C:/pro/emsdk/node/16.20.0_64bit/bin/node.exe -G Ninja
#cd C:\pro\github\bofwebrpc\vcprj
#vcprj.bat Web
#Launch VsCode
#CTRL-ALT-TARGET
#Select VcRun::WebNode::ut_bofwebrpc in debug icon

#Windows build
#cmake -DCMAKE_INSTALL_PREFIX=C:/bld/github-win/binaries -DCMAKE_TOOLCHAIN_FILE=C:/pro/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug -SC:/pro/github/bofwebrpc -BC:/bld/bofwebrpc-win

# git clone git@github.com:onbings/bofwebrpc.git
#Linux build
#cmake -DCMAKE_INSTALL_PREFIX=/home/bha/bld/github-win/binaries -DCMAKE_TOOLCHAIN_FILE=/home/bha/pro/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug -DRHEL:STRING="el9" -S/home/bha/pro/github/bofwebrpc -B/home/bha/bld/bofwebrpc-lin

#cmake --build .
#cmake --install . --config Debug

cmake_minimum_required(VERSION 3.28)

# Paranoid check : in-source generation is forbidden
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "You cannot generate your binary solution into your source directory !")
endif()
 
include(cmake/fixup_vs_crt.cmake)
include(cmake/vcpkg_install.cmake)
include(cmake/fetch_content.cmake)

project(bofwebrpc VERSION 2.4.2.8)
include(cmake/compile.cmake)
include(Dependencies.cmake)

option(${PROJECT_NAME_UPPER_UNDERSCORE}_BUILD_TESTS       "Build units tests of ${PROJECT_NAME_LOWER} project"                                  OFF)
option(${PROJECT_NAME_UPPER_UNDERSCORE}_ENABLE_VERSIONING "Enable library versioning naming scheme for ${PROJECT_NAME_LOWER} libraries"         ON)
option(${PROJECT_NAME_UPPER_UNDERSCORE}_FORCE_EXPORT_ALL  "Force exporting all symbols of ${PROJECT_NAME_LOWER} libraries (for shared library)" OFF)
set(${PROJECT_NAME_UPPER_UNDERSCORE}_EMSCRIPTEN_WASM_EXCEPTION ON)

# == BUILD ==
add_library(${PROJECT_NAME_LOWER_MINUS}
   ./include/bofwebrpc/bofgrpc.h
   ./src/bofgrpc.cpp
   ./include/bofwebrpc/bofwebrpc.h
   ./include/bofwebrpc/bofwebsocket.h
   ./src/bofwebsocket.cpp

   ./include/bofwebrpc/bofwebapp.h
   ./src/bofwebapp.cpp   
   ./include/bofwebrpc/bofwebserver.h
   ./src/bofwebserver.cpp
   ./include/bofwebrpc/bofwebclient.h
   ./src/bofwebclient.cpp
)
# Alias with the namespaced version (consistency)
add_library(ONBINGS::${PROJECT_NAME_LOWER_MINUS} ALIAS ${PROJECT_NAME_LOWER_MINUS})

target_compile_definitions(${PROJECT_NAME_LOWER_MINUS}
  PRIVATE
    -D${PROJECT_NAME_UPPER_UNDERSCORE}_EXPORTS=1	#in lower case to define the PROJECT_NAME_UPPER_UNDERSCORE BOFWEBRPC_EXPORT (upper case) in bofwebrpc_export.h
)
if (EMSCRIPTEN)
  include(cmake/emscripten.cmake)
else()
  target_link_libraries(${PROJECT_NAME_LOWER_MINUS} 
    PRIVATE
  )
endif()

target_include_directories(${PROJECT_NAME_LOWER_MINUS}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>       #for flat model (.h and .cpp in source dir)
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>		#for bofwebrpc_export.h install (below) 
    $<INSTALL_INTERFACE:include> 
)
target_link_libraries(${PROJECT_NAME_LOWER_MINUS}
  PRIVATE
    ONBINGS::bofstd
	  websockets
	  $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>
    protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite
    gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc++_alts		
    httplib::httplib
    #thrift::thrift 
    #thriftz::thriftz 
    ##thriftnb::thriftnb    
)

if(${PROJECT_NAME_UPPER_UNDERSCORE}_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# To avoid problem with EMSCRIPTEN compilation we only get third party libs which dont 
# depend on pthread lib. These one are get using the fetch_vcpkg_library function as
# they must be built with a strict control of options which is not possible with vcpkg 
# build in manifest mode 
message("==========================Start fetch for ${PROJECT_NAME}=========================")
#fetch_external_component_add(googletest "${LIST_OF_COMPONENT}" https://github.com/google/googletest release-1.12.1)
#make_external_library_available("${LIST_OF_COMPONENT}")
message("==========================End fetch for ${PROJECT_NAME}===========================")
#set_target_properties(gtest PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set_target_properties(gtest_main PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(cmake/export_version.cmake)
include(cmake/install.cmake)

#get_cmake_property(_variableNames VARIABLES)
#foreach(_variableName ${_variableNames})
#    message(STATUS "CMake variable: ${_variableName}=${${_variableName}}")
#endforeach()