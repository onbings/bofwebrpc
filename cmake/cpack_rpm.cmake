if (UNIX AND CMAKE_SYSTEM_NAME MATCHES Linux)
   find_program(RPMBUILD_FOUND rpmbuild)
   if (NOT RPMBUILD_FOUND)
      message(STATUS "rpmbuild not found")
   else()
      list(APPEND CPACK_GENERATOR        "RPM")
      set(CPACK_RPM_PACKAGE_NAME         "${PROJECT_NAME}${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
      set(CPACK_RPM_PACKAGE_RELEASE      "${VERSION_LABEL}")
      set(CPACK_RPM_PACKAGE_LICENSE      "Onbings")
      set(CPACK_RPM_PACAKGE_RELOCATABLE  ON)
      set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")

      if (NOT DEFINED VERSION_LABEL)
         set(VERSION_LABEL "alpha1" CACHE STRING "Version label : alpha1/beta1/rc1/release1" FORCE)
      endif()
      
      if (NOT DEFINED RHEL)
         message(FATAL_ERROR "You must define RHEL, add -DRHEL:STRING=\"el9\" or -DRHEL:STRING=\"el7\" to cmake command line")
      endif()

      if (${RHEL} STREQUAL "el9")
         set(OS_NAME rocky)
      elseif (${RHEL} STREQUAL "el7")
         set(OS_NAME centos)
      endif()

      set(rpm_releasePkgName   "${CPACK_RPM_PACKAGE_NAME}-${PROJECT_VERSION}-${VERSION_LABEL}-${RHEL}.${OS_NAME}.evs.rpm")
      set(rpm_debugPkgName     "${CPACK_RPM_PACKAGE_NAME}-debug-${PROJECT_VERSION}-${VERSION_LABEL}-${RHEL}.${OS_NAME}.evs.rpm")
      set(rpm_develPkgName     "${CPACK_RPM_PACKAGE_NAME}-devel-${PROJECT_VERSION}-${VERSION_LABEL}.rpm")
   
      set(CPACK_RPM_COMPONENT_INSTALL        ON)

      set(CPACK_RPM_RUNTIME_FILE_NAME        ${rpm_releasePkgName})
      set(CPACK_RPM_DEVEL_FILE_NAME          ${rpm_develPkgName})
      set(CPACK_RPM_DEBUG_FILE_NAME          ${rpm_debugPkgName})
         
      set(CPACK_RPM_RUNTIME_PACKAGE_NAME     ${CPACK_RPM_PACKAGE_NAME})
      set(CPACK_RPM_DEVEL_PACKAGE_NAME       ${CPACK_RPM_PACKAGE_NAME}-devel)
      set(CPACK_RPM_DEBUG_PACKAGE_NAME       ${CPACK_RPM_PACKAGE_NAME}-debug)

      set(CPACK_RPM_DEVEL_PACKAGE_REQUIRES   ${CPACK_RPM_PACKAGE_NAME})
      set(CPACK_RPM_DEBUG_PACKAGE_REQUIRES   ${CPACK_RPM_PACKAGE_NAME})

      set(CPACK_RPM_RUNTIME_PACKAGE_PREFIX   "/opt/onbings/${CPACK_RPM_PACKAGE_NAME}")
      set(CPACK_RPM_DEVEL_PACKAGE_PREFIX     "/opt/onbings/${CPACK_RPM_PACKAGE_NAME}")
      set(CPACK_RPM_DEBUG_PACKAGE_PREFIX     "/opt/onbings/${CPACK_RPM_PACKAGE_NAME}")

   #    set(CPACK_RPM_DEVEL_POST_INSTALL_SCRIPT_FILE       "${CMAKE_CURRENT_BINARY_DIR}/postinst")

   #    configure_file(
   #       ${CMAKE_CURRENT_LIST_DIR}/RpmPostInstallScript.in
   #       ${CMAKE_CURRENT_BINARY_DIR}/postinst @ONLY
   #    )

   endif()
else()
   message(STATUS "Bad cpack type (rpm) for this os (${CMAKE_SYSTEM_NAME})")
endif()
