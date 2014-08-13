# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

pkg_check_modules(ALU_GRID alugrid)
pkg_check_modules(UG_GRID libug)

SET( GRIDTYPE
	"SGRID" CACHE STRING
	"GRIDTYPE" )

SET( GRIDDIM
	"2" CACHE STRING
	"GRIDDIM" )

add_definitions( "-D${GRIDTYPE}" )
add_definitions( "-DGRIDDIM=${GRIDDIM}" )

if(UG_GRID_FOUND)
  include_sys_dir( ${UG_GRID_INCLUDE_DIRS} )
  link_directories( ${UG_GRID_LIBRARY_DIRS} )
  # althoug we use pkg_config to find ug we do not use UG_LIBRARIES,
  # because those listed in lubug.pc are wrong
  set(UGGRID_LIBS ugS2 ugS3 devS m)
endif(UG_GRID_FOUND)

set( ENABLE_UG OFF )
if( ${GRIDTYPE} MATCHES "UGGRID" )
  if( NOT UG_GRID_FOUND )
    message( FATAL_ERROR "Gridtype ${GRIDTYPE} requested, but libug was not found!" )
  endif( NOT UG_GRID_FOUND )
  add_definitions( "-DENABLE_UG=1" )
endif( ${GRIDTYPE} MATCHES "UGGRID" )

if(ALU_GRID_FOUND)
  include_sys_dir( ${ALU_GRID_INCLUDE_DIRS} )
  include_sys_dir( ${ALU_GRID_INCLUDE_DIRS}/serial )
  include_sys_dir( ${ALU_GRID_INCLUDE_DIRS}/duneinterface )
	if(ENABLE_MPI)
		include_sys_dir( ${ALUGRID_BASE_PATH}/include/parallel )
	endif(ENABLE_MPI)
  link_directories( ${ALU_GRID_LIBRARY_DIRS} )
  set(ALUGRID_LIBS ${ALU_GRID_LIBRARIES} )
endif(ALU_GRID_FOUND)

SET( ENABLE_ALUGRID OFF )
if( ${GRIDTYPE} MATCHES "ALUGRID" )
  if( NOT ALU_GRID_FOUND )
    message( FATAL_ERROR "Gridtype ${GRIDTYPE} requested, but alugrid was not found!" )
  endif( NOT ALU_GRID_FOUND )
	add_definitions( "-DENABLE_ALUGRID=1" )
endif( ${GRIDTYPE} MATCHES "ALUGRID" )

#SET( ENABLE_ALBERTA OFF )
#IF( ${GRIDTYPE} MATCHES "ALBERTAGRID" )
	#INCLUDE_SYS_DIR( 	${ALBERTA_GRID_BASE_PATH}/include
						#${ALBERTA_GRID_BASE_PATH}/include/alberta)
	#LINK_DIRECTORIES( "${ALBERTA_GRID_BASE_PATH}/lib"  )
	#SET( GRIDLIBS alberta_utilities alberta_2d dunealbertagrid_2d dunegrid)
	#SET( ENABLE_ALBERTA ON )
	#ADD_DEFINITIONS( -DENABLE_ALBERTA=1 -DDEBUG )
#ENDIF( ${GRIDTYPE} MATCHES "ALBERTAGRID" )

set( GRIDLIBS ${UGGRID_LIBS} ${ALUGRID_LIBS} )
