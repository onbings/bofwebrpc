if (Protobuf_FOUND)
  get_target_property(GRPC_SERVICE_CPP_PLUGIN gRPC::grpc_cpp_plugin LOCATION)    #C:\bld\evs-gbio\vcpkg_installed\evs-x64-windows-static\..\x64-windows\tools\grpc\grpc_cpp_plugin.exe
  get_target_property(GRPC_SERIALIZATOR protobuf::protoc LOCATION)       #C:\bld\evs-gbio\vcpkg_installed\x64-windows\tools\protobuf\protoc.exe

  file(MAKE_DIRECTORY ${GRPC_IDL_FILE_OUT_DIR})
  ## Define a custom target for protobuf code generation
  add_custom_target(generate_protobuf_code_of_${PROJECT_NAME_LOWER_MINUS} DEPENDS ${GRPC_IDL_FILES})  
  # Add dependencies to ensure code generation runs before building your executable
  add_dependencies(${PROJECT_NAME_LOWER_MINUS} generate_protobuf_code_of_${PROJECT_NAME_LOWER_MINUS})
  
  #GRPC_Serialization
      message("---GRPC_IDL_FILE_IN_DIR---->" ${GRPC_IDL_FILE_IN_DIR})
      message("---GRPC_IDL_FILE_OUT_DIR---->" ${GRPC_IDL_FILE_OUT_DIR})

  protobuf_generate(TARGET ${PROJECT_NAME_LOWER_MINUS}
    LANGUAGE cpp
    OUT_VAR GRPC_SERIALIZATION_GENERATED_FILES
    IMPORT_DIRS "${GRPC_IDL_FILE_IN_DIR}" 
    PROTOC_OUT_DIR "${GRPC_IDL_FILE_OUT_DIR}"
  )
  set_source_files_properties(${GRPC_SERIALIZATION_GENERATED_FILES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION on)
  message("---GRPC_SERIALIZATION_GENERATED_FILES--->" ${GRPC_SERIALIZATION_GENERATED_FILES})

  #GRPC_Server
  protobuf_generate(TARGET ${PROJECT_NAME_LOWER_MINUS}
    LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    OUT_VAR GRPC_SERVICE_GENERATED_FILES
    PLUGIN "protoc-gen-grpc=${GRPC_SERVICE_CPP_PLUGIN}"
    IMPORT_DIRS "${GRPC_IDL_FILE_IN_DIR}"
    PROTOC_OUT_DIR "${GRPC_IDL_FILE_OUT_DIR}"
  )
  set_source_files_properties(${GRPC_SERVICE_GENERATED_FILES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION on)
  message("---GRPC_SERVICE_GENERATED_FILES--------->" ${GRPC_SERVICE_GENERATED_FILES})

  target_include_directories(${PROJECT_NAME_LOWER_MINUS} PUBLIC "$<BUILD_INTERFACE:${GRPC_IDL_FILE_OUT_DIR}>")
  
else()
  message(FATAL_ERROR "Protobuf package no found. Is protocolbuffer/grpc in your dependencies ?")
endif()

if (DEFINED Flatbuffers_FOUND)
  
    file(MAKE_DIRECTORY ${GRPC_FBS_IDL_FILE_OUT_DIR})
    add_custom_command(
      #check if needed OUTPUT 
      #check if needed ${GRPC_FBS_IDL_FILE_OUT_DIR}/${PROJECT_NAME_LOWER_MINUS}_generated.h  
      #check if needed ${GRPC_FBS_IDL_FILE_OUT_DIR}/${PROJECT_NAME_LOWER_MINUS}.grpc.fb.cc
      #check if needed ${GRPC_FBS_IDL_FILE_OUT_DIR}/${PROJECT_NAME_LOWER_MINUS}.grpc.fb.h
      COMMENT  "Running custom command for fbs header generation..."
      COMMAND 
        ${FLATC_EXECUTABLE}
        --cpp
        -o ${GRPC_FBS_IDL_FILE_OUT_DIR} 
        #--gen-mutable
        #--gen-object-api
        #--reflect-names
        -I ${GRPC_FBS_IDL_FILE_IN_DIR}
        #--binary-json
        --grpc
        ${GRPC_FBS_IDL_FILE_IN_DIR}/${PROJECT_NAME_LOWER_MINUS}.fbs
      DEPENDS
        ${GRPC_FBS_IDL_FILE_IN_DIR}/${PROJECT_NAME_LOWER_MINUS}.fbs
      #VERBOSE=1
    )
    target_include_directories(${PROJECT_NAME_LOWER_MINUS} PUBLIC "$<BUILD_INTERFACE:${GRPC_FBS_IDL_FILE_OUT_DIR}>")  
else()
  message("Flatbuffers package no found->Use native grpc serialization")
endif()


