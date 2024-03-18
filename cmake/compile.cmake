# Programs used for integration testing should not be included in code coverage computation as they are test programs themselves.
set(EXTRA_COVERAGE_EXCLUSION "\'${CMAKE_CURRENT_SOURCE_DIR}/integration/*\'")

# Some naming conventions either requires lower or upper case.
# And some don't like underscore. So already prepare all those variables
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
string(REPLACE "-" "_" PROJECT_NAME_UNDERSCORE       ${PROJECT_NAME})
string(REPLACE "-" "_" PROJECT_NAME_LOWER_UNDERSCORE ${PROJECT_NAME_LOWER})
string(REPLACE "_" "-" PROJECT_NAME_LOWER_MINUS      ${PROJECT_NAME_LOWER})
string(REPLACE "-" "_" PROJECT_NAME_UPPER_UNDERSCORE ${PROJECT_NAME_UPPER})
if (PROJECT_IS_TOP_LEVEL)
   file(REMOVE ${CMAKE_BINARY_DIR}/DepManifest.txt)
endif()

# == NON TRANSITIVE PROPERTIES ==
# Non transitive properties are properties that are not
# usage requirements. It means properties that only need
# to be set for building this target only. As such they
# don't need to propagated outside.
# For convenience, use global initializer for those

# Disable compiler extension to maximize portability
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_DEBUG_POSTFIX _d)

if ((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo"))
if(MSVC)
else'()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
endif ()
endif ()

#set general thrift/grpc variable (used by cmake/grpc.cmake and cmake/thrift.cmake)
set(THRIFT_GEN_DIR_CPP "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}/thrift")
set(GRPC_PROTO_FILE_IN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/idl)
set(GRPC_PROTO_FILE_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/grpc)

# Make sure MSVC runtime is consistent accross
# all languages (i.e. CXX, CUDA, etc, ...)
# cfr : https://gitlab.kitware.com/cmake/cmake/-/issues/19428
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") 
set_msvc_runtime_library()
# Fix rpath from installed vpckg dependencies
patch_vcpkg_installed_rpath()

find_package(Doxygen)
message("=====================> Doxygen compiler: " ${DOXYGEN_EXECUTABLE})
if(${DOXYGEN_EXECUTABLE} STREQUAL "doxygen-NOTFOUND")
  message("WARNING: Could not find doxygen executable. Is doxygen installed ?")
endif()

# For shared libraries we need to compile with "-fPIC"
# But for static libraries user can choose
if(NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE  ${BUILD_SHARED_LIBS})
endif()

# API visibility control
if(${PROJECT_NAME_UPPER_UNDERSCORE}_FORCE_EXPORT_ALL)

  if(MSVC AND ${BUILD_SHARED_LIBS})
    message(WARNING "\nYou requested to force exporting ALL symbols "
                    "in a Windows DLL through CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS\n"
                    "This is a hack that has limitations. Use it as a temporary workaround")
  endif()

  # By "default",
  #  - GCC and Clang export everything (i.e. nothing to do)
  #  - MSVC hides everything and requires explicit listing
  #
  # This command tells CMake to generate such listing
  # for all the symbols found in the binary
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS   ON)

else()

  # We only want the API symbols to be visible
  # Tell CMake to pass the proper flags to hide
  # everything that is not explicitly marked
  set(CMAKE_CXX_VISIBILITY_PRESET         hidden)
  set(CMAKE_CXX_VISIBILITY_INLINES_HIDDEN ON)

endif()
