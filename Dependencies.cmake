include(CMakeFindDependencyMacro)

macro(find_package_dependency)
  # When loading the *Config.cmake we should
  # call find_dependency which is just a wrapper
  # around find_package to display better
  # messages to the user. When directly dealing
  # with our CMakeLists.txt, we should call
  # find_package directly
  if(FROM_CONFIG_FILE)
     find_dependency(${ARGN})
  else()
     find_package(${ARGN})
  endif()
endmacro()

# == OPTIONAL DEPENDENCIES ==

# == REQUIRED DEPENDENCIES ==
#find_package(GTest CONFIG REQUIRED)
find_package(bofstd CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)
find_package(mdns CONFIG REQUIRED)
if (EMSCRIPTEN)
else()
find_package(libwebsockets CONFIG REQUIRED)
find_package(libuv CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
endif()