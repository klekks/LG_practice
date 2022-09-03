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

cmake_minimum_required (VERSION 3.5)

#-------------------------------------------------------------------------------
#
# Version number
#
#-------------------------------------------------------------------------------

# Read version string from VERSION environment variable or fetch from Git.
set (VERSION "0.1-1-gc3579c5")

string (REGEX MATCH "^([0-9]+)\.([0-9]+)\-([0-9]+)\-" VERSION_PARTS ${VERSION})

set (VERSION_MAJOR ${CMAKE_MATCH_1})
set (VERSION_MINOR ${CMAKE_MATCH_2})
set (VERSION_PATCH ${CMAKE_MATCH_3})

# Set deb package version
set (PACKAGE_VERSION "${VERSION}-1")
# Set internal cmake project version string
set (PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message("Building package version: ${PACKAGE_VERSION}")
message("Set cmake project version: ${PROJECT_VERSION}")

#-------------------------------------------------------------------------------
#
# Project setup
#
#-------------------------------------------------------------------------------

project ("spatium-mock-speech" VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

if (NOT CMAKE_BUILD_TYPE)
   set (CMAKE_BUILD_TYPE "Release")
   message (STATUS "Build type not specified: default is Release")
endif ()

message (STATUS "CMAKE_VERSION=${CMAKE_VERSION}")
message (STATUS "Build type: ${CMAKE_BUILD_TYPE}")

message (STATUS "Configuring ${CMAKE_PROJECT_NAME} ${PACKAGE_VERSION}")
message (STATUS "PROJECT_BINARY_DIR ${PROJECT_BINARY_DIR}")
message (STATUS "PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}")
message (STATUS "CONFIG ${CONFIG}")

#-------------------------------------------------------------------------------
#
# Compiler flags
#
#-------------------------------------------------------------------------------

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

add_compile_options (
     "-Wall" "-Wextra" "-fexceptions"
     "$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
)

#-------------------------------------------------------------------------------
#
# Dependencies
#
#-------------------------------------------------------------------------------

find_package (PkgConfig REQUIRED)

# jsoncpp
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
message (STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
include (LibFindMacros)
find_package (Jsoncpp REQUIRED)

# Boost
set (Boost_USE_STATIC_LIBS OFF)
SET (BOOST_MIN_VERSION "1.71.0")
SET (Boost_VERBOSE "1")
find_package (Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS thread log log_setup program_options)

#add_definitions (-DBOOST_LOG_DYN_LINK -DBOOST_LOG_USE_NATIVE_SYSLOG)
add_definitions (-DBOOST_LOG_USE_NATIVE_SYSLOG)

#-------------------------------------------------------------------------------
#
# Source files
#
#-------------------------------------------------------------------------------

include_directories(${PROJECT_SOURCE_DIR}/../src)

file (GLOB PROJECT_SRCLIST
      ${PROJECT_SOURCE_DIR}/../src/config/*.cpp
      ${PROJECT_SOURCE_DIR}/../src/*.cpp
)

add_executable (spatium-mock-speech ${PROJECT_SRCLIST})

message("Boost headers path: ${Boost_INCLUDE_DIRS}")

set (PROJECT_INCLUDES
     ${Boost_INCLUDE_DIRS}
     ${Jsoncpp_INCLUDE_DIRS}
)

target_include_directories (spatium-mock-speech PRIVATE ${PROJECT_INCLUDES})

#-------------------------------------------------------------------------------
#
# Linker
#
#-------------------------------------------------------------------------------

# Specify libraries to link against.
set (PROJECT_LIBS
     ${Boost_LIBRARIES}
     ${Jsoncpp_LIBRARY}
)

target_link_libraries (spatium-mock-speech ${PROJECT_LIBS})

#-------------------------------------------------------------------------------
#
#  Packaging / Installation
#
#-------------------------------------------------------------------------------

include (${CMAKE_SOURCE_DIR}/CPackConfig.cmake)
