#-------------------------------------------------------------------------------
#
# Copyright (C) 2021 LG Electronics Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#-------------------------------------------------------------------------------

### general settings
set (CMAKE_INSTALL_PREFIX "/")
message (STATUS "CMAKE_INSTALL_PREFIX='${CMAKE_INSTALL_PREFIX}'")

set (CPACK_PACKAGE_NAME "spatium-mock-speech")
set (CPACK_PACKAGE_VERSION "${PACKAGE_VERSION}")

set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/../LICENSE")
set (CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/../README")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/../DESCRIPTION")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Spatium Mock AI Speech Service")

set (CPACK_PACKAGE_VENDOR "LG Electronics")
set (CPACK_PACKAGE_CONTACT "andrey.volykhin@lge.com")
#set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set (CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}")

### versions
set (CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

### deb generator
set (CPACK_GENERATOR "DEB")
set (CPACK_DEBIAN_PACKAGE_SECTION "net")
set (CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set (CPACK_DEBIAN_PACKAGE_DEPENDS "libcurl4 (>= 7.74.0)")

set (CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})

#
# Set the architecture
#
find_program (DPKG_CMD dpkg)
if (NOT DPKG_CMD)
  message (FATAL_ERROR "Can not find dpkg in your path.")
endif (NOT DPKG_CMD)
execute_process (COMMAND "${DPKG_CMD}" --print-architecture
                 OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
                 OUTPUT_STRIP_TRAILING_WHITESPACE
)

set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

message (STATUS "CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}")
message (STATUS "CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")

install (DIRECTORY
         DESTINATION /etc/spatium/mock-speech
)

install (DIRECTORY
         DESTINATION /usr/share/spatium/mock-speech
)

install (DIRECTORY
         DESTINATION /var/log/spatium
)

install (DIRECTORY
         DESTINATION /var/run/spatium
)


install (TARGETS spatium-mock-speech RUNTIME
         DESTINATION /usr/share/spatium/mock-speech)

file (GLOB_RECURSE PROJECT_CONF ../conf/*)

install (FILES ${PROJECT_CONF}
         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
         DESTINATION /etc/spatium/mock-speech
)
message (STATUS "PROJECT_CONF:${PROJECT_CONF}")
install (FILES "${CMAKE_SOURCE_DIR}/../deb_package/${CPACK_PACKAGE_NAME}.logrotate"
         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
         DESTINATION /etc/logrotate.d
         RENAME "${CPACK_PACKAGE_NAME}"
)

install (PROGRAMS "${CMAKE_SOURCE_DIR}/../deb_package/${CPACK_PACKAGE_NAME}.init"
         PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
         DESTINATION /etc/init.d
         RENAME "${CPACK_PACKAGE_NAME}"
)

file (COPY "${CMAKE_SOURCE_DIR}/../deb_package/${CPACK_PACKAGE_NAME}.prerm"
      DESTINATION "${CMAKE_BINARY_DIR}/package")
file (RENAME "${CMAKE_BINARY_DIR}/package/${CPACK_PACKAGE_NAME}.prerm" "${CMAKE_BINARY_DIR}/package/prerm")

file (COPY "${CMAKE_SOURCE_DIR}/../deb_package/${CPACK_PACKAGE_NAME}.postinst"
      DESTINATION "${CMAKE_BINARY_DIR}/package")
file (RENAME "${CMAKE_BINARY_DIR}/package/${CPACK_PACKAGE_NAME}.postinst" "${CMAKE_BINARY_DIR}/package/postinst")

file (COPY "${CMAKE_SOURCE_DIR}/../deb_package/${CPACK_PACKAGE_NAME}.postrm"
      DESTINATION "${CMAKE_BINARY_DIR}/package")
file (RENAME "${CMAKE_BINARY_DIR}/package/${CPACK_PACKAGE_NAME}.postrm" "${CMAKE_BINARY_DIR}/package/postrm")

file (WRITE "${CMAKE_BINARY_DIR}/package/conffiles" "/etc/init.d/${CPACK_PACKAGE_NAME}\n")

set (CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_BINARY_DIR}/package/conffiles;${CMAKE_BINARY_DIR}/package/prerm;${CMAKE_BINARY_DIR}/package/postinst;${CMAKE_BINARY_DIR}/package/postrm")
set (CPACK_OUTPUT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/debPackageConfig.cmake")

add_custom_target (deb_package
    COMMAND "${CMAKE_CPACK_COMMAND}"
    "--config" "${CMAKE_CURRENT_SOURCE_DIR}/debPackageConfig.cmake")

include(CPack)
