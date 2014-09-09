# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

SET(GRIDTYPE
    "SGRID"
    CACHE STRING "GRIDTYPE")

SET(GRIDDIM
    "2"
    CACHE STRING "GRIDDIM")

add_definitions("-DGRIDDIM=${GRIDDIM}")
add_definitions("-DGRIDTYPE=${GRIDTYPE}")
add_definitions("-D${GRIDTYPE}")

if(CMAKE_WITH_AUTOTOOLS)
        pkg_check_modules(ALUGRID alugrid)
  pkg_check_modules(UG_GRID libug)

  if(UG_GRID_FOUND)
    include_sys_dir(${UG_GRID_INCLUDE_DIRS})
    link_directories(${UG_GRID_LIBRARY_DIRS})
    # althoug we use pkg_config to find ug we do not use UG_LIBRARIES,
    # because those listed in lubug.pc are wrong
    set(UGGRID_LIBS ugS2 ugS3 devS m)
  endif(UG_GRID_FOUND)
  set(ENABLE_UG OFF)
  if(${GRIDTYPE} MATCHES "UGGRID")
    if(NOT UG_GRID_FOUND)
      message(FATAL_ERROR "Gridtype ${GRIDTYPE} requested, but libug was not found!")
    endif(NOT UG_GRID_FOUND)
    add_definitions("-DENABLE_UG=1")
  endif(${GRIDTYPE} MATCHES "UGGRID")

  if(ALUGRID_FOUND)
    include_sys_dir(${ALUGRID_INCLUDE_DIRS})
    include_sys_dir(${ALUGRID_INCLUDE_DIRS}/serial)
    include_sys_dir(${ALUGRID_INCLUDE_DIRS}/duneinterface)
    if(ENABLE_MPI)
      include_sys_dir(${ALUGRID_BASE_PATH}/include/parallel)
    endif(ENABLE_MPI)
    link_directories(${ALUGRID_LIBRARY_DIRS})
    set(ALUGRID_LIBS ${ALUGRID_LIBRARIES})
  endif(ALUGRID_FOUND)
  set(ENABLE_ALUGRID OFF)

  #SET( ENABLE_ALBERTA OFF )
  #IF( ${GRIDTYPE} MATCHES "ALBERTAGRID" )
  	#INCLUDE_SYS_DIR( 	${ALBERTA_GRID_BASE_PATH}/include
  						#${ALBERTA_GRID_BASE_PATH}/include/alberta)
  	#LINK_DIRECTORIES( "${ALBERTA_GRID_BASE_PATH}/lib"  )
  	#SET( GRIDLIBS alberta_utilities alberta_2d dunealbertagrid_2d dunegrid)
  	#SET( ENABLE_ALBERTA ON )
  	#ADD_DEFINITIONS( -DENABLE_ALBERTA=1 -DDEBUG )
  #ENDIF( ${GRIDTYPE} MATCHES "ALBERTAGRID" )

  set(GRIDLIBS ${UGGRID_LIBS} ${ALUGRID_LIBS} )

endif(CMAKE_WITH_AUTOTOOLS)


if(${GRIDTYPE} MATCHES "ALUGRID")
  if(NOT ALUGRID_FOUND)
    message( FATAL_ERROR "Gridtype ${GRIDTYPE} requested, but alugrid was not found!" )
  endif(NOT ALUGRID_FOUND)
  add_definitions("-DENABLE_ALUGRID=1")
endif(${GRIDTYPE} MATCHES "ALUGRID")
