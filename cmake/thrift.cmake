if (DEFINED thrift_FOUND)
# Create output directory
  file(MAKE_DIRECTORY ${THRIFT_GEN_DIR_CPP})
 
# Generate files
  add_custom_command(
	OUTPUT  ${THRIFT_GENERATED_FILES}
	DEPENDS ${THRIFT_IDL_FILES}
	COMMENT "(thrift) Generating CPP bindings depends on  ${THRIFT_IDL_FILES}"
	COMMAND echo -e ${THRIFT_EXECUTABLE} -I ${CMAKE_CURRENT_SOURCE_DIR} -r -gen cpp:moveable_types -out ${THRIFT_GEN_DIR_CPP} ${THRIFT_ROOT_IDL_FILE}
	COMMAND ${THRIFT_EXECUTABLE} -I ${CMAKE_CURRENT_SOURCE_DIR} -r -gen cpp:moveable_types -out ${THRIFT_GEN_DIR_CPP} ${THRIFT_ROOT_IDL_FILE}
	VERBATIM
  )
else()
  message("Thrift package no found. Is Thrift in your dependencies ?")
endif()
