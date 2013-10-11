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

set( ENABLE_UG OFF )
if( ${GRIDTYPE} MATCHES "UGGRID" )
  if( NOT UG_GRID_FOUND )
    message( FATAL_ERROR "Gridtype ${GRIDTYPE} requested, but libug was not found!" )
  endif( NOT UG_GRID_FOUND )
  include_sys_dir( ${UG_GRID_INCLUDE_DIRS} )
  link_directories( ${UG_GRID_LIBRARY_DIRS} )
  # althoug we use pkg_config to find ug we do not use UG_LIBRARIES,
  # because those listed in lubug.pc are wrong
  set( GRIDLIBS ugS2 ugS3 devS m )
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
