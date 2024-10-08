include(GoogleTest)

set(SOURCES_FILES
	include/gtestrunner.h
	src/main.cpp

	src/ut_webapp.cpp
)

# For Visual Studio
source_group("Tst"		 FILES ${SOURCES_FILES})

# == BUILD ==
if (MSVC)
    add_compile_options(/bigobj)
else()
endif()

# Create target
set (UT_NAME ${PROJECT_NAME}-tests)
add_executable(${UT_NAME}
  ${SOURCES_FILES}
)
# Includes
target_include_directories(${UT_NAME}
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>       #for flat model (.h and .cpp in source dir)
)

target_link_libraries(${UT_NAME}
  PRIVATE
    ONBINGS::bofwebrpc
	#vcpkg 
    GTest::gtest
	#Fetch 	GTest::gtest_main
    ONBINGS::bofstd
	
)

# Object library is not a shared library
# void import directive only for linking
target_compile_definitions(${UT_NAME}
  PRIVATE
    -DBOFWEBRPC_STATIC_DEFINE=1
)

# Add test and allow them to run in //
gtest_discover_tests(${UT_NAME}
  XML_OUTPUT_DIR     ${CMAKE_BINARY_DIR}/GTestReports/
  TEST_PREFIX        ${PROJECT_NAME_LOWER_UNDERSCORE}_tests
   DISCOVERY_TIMEOUT 120
   DISCOVERY_MODE    PRE_TEST
   WORKING_DIRECTORY $<TARGET_FILE_DIR:${UT_NAME}>
)

# If you plan on running your unit tests with Valgrind
# it might happen that you need to add 'suppressions'
# to silence known issues. valgrind accepts suppressions
# by taking a file (--log-file=...). The variable
# MEMORYCHECK_SUPPRESSIONS_FILE is not a CMake variable
# but can be used as a convenience to accumulate suppressions
# from all the unit tests. Consequently, if you configure
# valgrind to use that file and define this variable, you will
# have the expected result.
if(MEMORYCHECK_SUPPRESSIONS_FILE)
  file(READ   valgrind_suppression.txt           CONTENT)
  file(APPEND ${MEMORYCHECK_SUPPRESSIONS_FILE} ${CONTENT})
endif()

#message("=============>cp 1:${CMAKE_INSTALL_PREFIX} 2:${CMAKE_CURRENT_BINARY_DIR}" 3:${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#add_custom_command(TARGET ${TargetName} POST_BUILD
add_custom_command(TARGET ${UT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/web-srv.json" ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/web-clt.json" ${CMAKE_CURRENT_BINARY_DIR}
)