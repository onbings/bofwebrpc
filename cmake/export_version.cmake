# == VERSIONING ==
get_target_property(target_type ${PROJECT_NAME_LOWER_MINUS} TYPE)
if (target_type STREQUAL "EXECUTABLE")
else()
	# Set version
	if(${PROJECT_NAME_UPPER_UNDERSCORE}_ENABLE_VERSIONING)
	  # Under Linux this function will create the
	  # library file with the full version name
	  # and the proper symlinks.
	  # e.g.
	  #  libbofstd.so.1.0.1
	  #  libbofstd.so.1 --> libbofstd.so.1.0.1
	  set_target_properties(${PROJECT_NAME_LOWER_MINUS}  PROPERTIES
		VERSION   ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
	  )
	endif()

	# == API management ==
	include(GenerateExportHeader)

	# This CMake function generates a header file (i.e. evs-gfl_export.h)
	# that contains all the proper 'defines' to import/export symbols
	# (e.g. EVS_GLF_EXPORT) for shared libraries. When building as static
	# those macros are simply blank. This file will be by default generated
	# into ${CMAKE_CURRENT_BINARY_DIR} (see target_include_directories above)
	generate_export_header(${PROJECT_NAME_LOWER_MINUS} EXPORT_MACRO_NAME ${PROJECT_NAME_UPPER_UNDERSCORE}_EXPORT)
endif()