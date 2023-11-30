# Locate the Bullet library
#
# This module defines the following variables:
#
# BULLET_COLLISION_LIBRARY the name of the Collision library;
# BULLET_DYNAMICS_LIBRARY the name of the Dynamics library;
# BULLET_LINEAR_MATH the name of the LinearMath library;
# BULLET_INCLUDE_DIR where to find Bullet include files.
# BULLET_FOUND true if all libs and include directories have been found.
#
# To help locate the library and include file, you can define a
# variable called BULLET_ROOT which points to the root of the Bullet library
# installation.
#

set( _bullet3_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/include"
"C:/Program Files (x86)/Bullet/include" )
set( _bullet3_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib"
"C:/Program Files (x86)/Bullet/lib-msvc110" )

# Check environment for root search directory
set( _bullet3_ENV_ROOT $ENV{BULLET_ROOT} )
if( NOT BULLET_ROOT AND _bullet3_ENV_ROOT )
	set(BULLET_ROOT ${_bullet3_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( BULLET_ROOT )
	list( INSERT _bullet3_HEADER_SEARCH_DIRS 0 "${BULLET_ROOT}/include" )
	list( INSERT _bullet3_LIB_SEARCH_DIRS 0 "${BULLET_ROOT}/lib/Debug"  "${BULLET_ROOT}/lib/Release")
endif()

# Search for the header
FIND_PATH(BULLET_INCLUDE_DIR "btBulletDynamicsCommon.h"
PATHS ${_bullet3_HEADER_SEARCH_DIRS} )

INCLUDE(FindPackageHandleStandardArgs)

# Search for Bullet Collision
FIND_LIBRARY(BULLET_COLLISION_LIBRARY NAMES BulletCollision
PATHS ${_bullet3_LIB_SEARCH_DIRS} )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bullet DEFAULT_MSG
BULLET_COLLISION_LIBRARY BULLET_INCLUDE_DIR)

# Debug version
FIND_LIBRARY(BULLET_COLLISION_LIBRARY_DEBUG NAMES BulletCollision_Debug
PATHS ${_bullet3_LIB_SEARCH_DIRS} )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bullet DEFAULT_MSG
BULLET_COLLISION_LIBRARY_DEBUG BULLET_INCLUDE_DIR)

# Search for Bullet Dynamics
FIND_LIBRARY(BULLET_DYNAMICS_LIBRARY NAMES BulletDynamics
PATHS ${_bullet3_LIB_SEARCH_DIRS} )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bullet DEFAULT_MSG
BULLET_DYNAMICS_LIBRARY BULLET_INCLUDE_DIR)

# Debug version
FIND_LIBRARY(BULLET_DYNAMICS_LIBRARY_DEBUG NAMES BulletDynamics_Debug
PATHS ${_bullet3_LIB_SEARCH_DIRS} )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bullet DEFAULT_MSG
BULLET_DYNAMICS_LIBRARY_DEBUG BULLET_INCLUDE_DIR)

# Search for Bullet Linear math
FIND_LIBRARY(BULLET_LINEAR_MATH NAMES LinearMath
PATHS ${_bullet3_LIB_SEARCH_DIRS} )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bullet DEFAULT_MSG
BULLET_LINEAR_MATH BULLET_INCLUDE_DIR)

# Debug version
FIND_LIBRARY(BULLET_LINEAR_MATH_DEBUG NAMES LinearMath_Debug
PATHS ${_bullet3_LIB_SEARCH_DIRS} )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bullet DEFAULT_MSG
BULLET_LINEAR_MATH_DEBUG BULLET_INCLUDE_DIR)