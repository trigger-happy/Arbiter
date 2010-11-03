# - Find apr
# Find the native APR includes and library
#
#  APR_INCLUDE_DIRS - where to find apr.h, etc.
#  APR_LIBRARIES    - List of libraries when using apr.
#  APR_FOUND        - True if apr found.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

IF (APR_INCLUDE_DIR)
  # Already in cache, be silent
  SET(APR_FIND_QUIETLY TRUE)
ENDIF (APR_INCLUDE_DIR)

FIND_PATH(APR_INCLUDE_DIR apr.h apr-1/apr.h)

SET(APR_NAMES apr-1)
FIND_LIBRARY(APR_LIBRARY NAMES ${APR_NAMES} )
MARK_AS_ADVANCED( APR_LIBRARY APR_INCLUDE_DIR )

# Per-recommendation
SET(APR_INCLUDE_DIRS "${APR_INCLUDE_DIR}")
SET(APR_LIBRARIES    "${APR_LIBRARY}")

# handle the QUIETLY and REQUIRED arguments and set APR_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(APR DEFAULT_MSG APR_LIBRARIES APR_INCLUDE_DIRS)

