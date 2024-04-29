include(FetchContent)

#first call this one for each component AND AFTER call ONCE make_external_component_available
function(fetch_external_component_add COMPONENT_NAME LIST_OF_COMPONENT GIT_URL GIT_TAG)
    message(STATUS "Start of fetch external component '${COMPONENT_NAME}' from ${GIT_URL} at tag ${GIT_TAG}:")
    set(COMPONENT_DIR "${CMAKE_CURRENT_LIST_DIR}/${COMPONENT_NAME}")
	list(FIND LIST_OF_COMPONENT ${COMPONENT_NAME} INDEX)
	if(INDEX EQUAL -1)
		if(EXISTS "${COMPONENT_DIR}/.git")
			execute_process(
				COMMAND git -C "${COMPONENT_DIR}" status --porcelain  --untracked-files=no
				OUTPUT_VARIABLE GIT_STATUS
				RESULT_VARIABLE GIT_STATUS_RESULT
			)

			if(GIT_STATUS_RESULT EQUAL 0 AND NOT "${GIT_STATUS}" STREQUAL "")
				message(FATAL_ERROR "Uncommitted changes ddd found in: ${COMPONENT_DIR}\n${GIT_STATUS}")
			endif()
		endif()
	
		set(FETCHCONTENT_QUIET OFF)
		FetchContent_Declare(
			${COMPONENT_NAME}
			GIT_REPOSITORY ${GIT_URL}
			GIT_TAG ${GIT_TAG}
			SOURCE_DIR "${COMPONENT_DIR}"
		)
		list(LENGTH LIST_OF_COMPONENT LIST_SIZE)
		if(LIST_SIZE EQUAL 0)
			file(APPEND ${CMAKE_BINARY_DIR}/DepManifest.txt "FetchContent Dependencies for ${PROJECT_NAME}:\n")
		endif()
		file(APPEND ${CMAKE_BINARY_DIR}/DepManifest.txt "${COMPONENT_NAME}\t${GIT_TAG}\t${GIT_URL}\n")
		list(APPEND LIST_OF_COMPONENT ${COMPONENT_NAME})  # Append component name to the provided list
		set(LIST_OF_COMPONENT "${LIST_OF_COMPONENT}" PARENT_SCOPE)
		message(STATUS "The component '${COMPONENT_NAME}' has been added to the list")
	else()
	   message(STATUS "The component '${COMPONENT_NAME}' is already present in the list")
	endif()
    message(STATUS "End of of fetch external component add '${COMPONENT_NAME}'")
endfunction()

function(make_external_component_available LIST_OF_COMPONENT)
    message(STATUS "make_external_component_available for the following component:")
    foreach(COMPONENT_NAME ${LIST_OF_COMPONENT})
        message("${COMPONENT_NAME}")
#		FetchContent_GetProperties(${COMPONENT_NAME})
#		message(STATUS "----------make_external_component_available------->${COMPONENT_NAME}_POPULATED")
    endforeach()
    FetchContent_MakeAvailable("${LIST_OF_COMPONENT}")
endfunction()

# Function to list CMake variables with a given prefix
# list_cmake_vars_with_prefix("CMAKE_")
# list_cmake_vars_with_prefix("CXX_")
function(list_cmake_vars_with_prefix _prefix)
    message(STATUS "Listing CMake variables with prefix '${_prefix}':")
    get_cmake_property(_variable_names VARIABLES)
    foreach(_variable_name IN LISTS _variable_names)
        string(FIND "${_variable_name}" "${_prefix}" _prefix_length)
        if (_prefix_length EQUAL 0)
            message(STATUS "${_variable_name} = ${${_variable_name}}")
        endif()
    endforeach()
    message(STATUS "End of listing")
endfunction()

function(list_cmake_vars_containing _str)
    message(STATUS "Listing CMake variables which contains '${_str}':")
    string(TOLOWER "${_str}" _str_lower)
    get_cmake_property(_variable_names VARIABLES)
    foreach(_variable_name IN LISTS _variable_names)
        string(TOLOWER "${_variable_name}" _variable_lower)
        string(FIND "${_variable_lower}" "$_str_lower}" pos)    
        string(FIND "${_variable_name}" "${_str}" pos)
        if (NOT pos EQUAL -1)
            message(STATUS "${_variable_name} = ${${_variable_name}}")
        endif()
    endforeach()
    message(STATUS "End of listing")
endfunction()

#get_cmake_property(_variable_names VARIABLES)
#foreach(_variable_name IN LISTS _variable_names)
#   message(STATUS "${_variable_name} = ${${_variable_name}}")
#endforeach()	