SET( GRIDTYPE
	"SGRID" CACHE STRING
	"GRIDTYPE" )

SET( GRIDDIM
	"2" CACHE STRING
	"GRIDDIM" )

set( ENABLE_UG OFF )
if( ${GRIDTYPE} MATCHES "UGGRID" )
  if( NOT UG_GRID_FOUND )
    message( FATAL_ERROR "Gridtype ${GRIDTYPE} requested, but libug was not found!" )
  endif( NOT UG_GRID_FOUND )
  # ug needs Xt, but the libXt.so seems to be missing sometimes
  SET( TMP_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} )
  SET( CMAKE_FIND_LIBRARY_SUFFIXES ${TMP_CMAKE_FIND_LIBRARY_SUFFIXES} ".so.6" )
  find_library( XT_FOUND Xt )
  SET( CMAKE_FIND_LIBRARY_SUFFIXES ${TMP_CMAKE_FIND_LIBRARY_SUFFIXES} )
  if( NOT XT_FOUND )
    message( FATAL_ERROR "Gridtype UGGRID requested, but libXt was not found!" )
  endif( NOT XT_FOUND )
  include_sys_dir( ${UG_GRID_INCLUDE_DIRS} )
  link_directories( ${UG_GRID_LIBRARY_DIRS} )
  set( GRIDLIBS ugS2 ugS3 devX ugL2 ugL3 SM ICE X11 ${XT_FOUND} )
  add_definitions( "-DENABLE_UG=1" )
endif( ${GRIDTYPE} MATCHES "UGGRID" )

SET( ENABLE_ALUGRID OFF )
if( ${GRIDTYPE} MATCHES "ALUGRID" )
  if( NOT ALU_GRID_FOUND )
    message( FATAL_ERROR "Gridtype ${GRIDTYPE} requested, but alugrid was not found!" )
  endif( NOT ALU_GRID_FOUND )
  include_sys_dir( ${ALU_GRID_INCLUDE_DIRS} )
  include_sys_dir( ${ALU_GRID_INCLUDE_DIRS}/serial )
  include_sys_dir( ${ALU_GRID_INCLUDE_DIRS}/duneinterface )
  link_directories( ${ALU_GRID_LIBRARY_DIRS} )
  set( GRIDLIBS ${ALU_GRID_LIBRARIES} )
	if( ENABLE_MPI )
		include_sys_dir( ${ALUGRID_BASE_PATH}/include/parallel )
	endif( ENABLE_MPI )
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
