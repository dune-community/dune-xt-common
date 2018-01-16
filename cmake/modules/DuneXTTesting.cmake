function(dune_xt_add_test)
    # parse arguments
    set(OPTIONS EXPECT_COMPILE_FAIL EXPECT_FAIL SKIP_ON_77 COMPILE_ONLY)
    set(SINGLEARGS NAME TIMEOUT SCRIPT)
    set(MULTIARGS SOURCES COMPILE_DEFINITIONS COMPILE_FLAGS LINK_LIBRARIES CMD_ARGS MPI_RANKS CMAKE_GUARD)
    cmake_parse_arguments(XTTEST "${OPTIONS}" "${SINGLEARGS}" "${MULTIARGS}" ${ARGN})

    if(XTTEST_UNPARSED_ARGUMENTS)
        message(WARNING "dune_xt_add_test: Encountered unparsed arguments: This often indicates typos in named arguments: ${XTTEST_UNPARSED_ARGUMENTS}" )
    endif()

    # construct a string containg DEBUG to pass the debug flag to the other macros
    set(DEBUG "")
    if(XTTEST_DEBUG)
        set(DEBUG "DEBUG")
    endif()

    if(NOT XTTEST_SOURCES)
        message(FATAL_ERROR "Specify either the SOURCES argument!")
    endif()

    if(NOT XTTEST_SCRIPT)
        set(XTTEST_SCRIPT dune_xt_execute.py)
    endif()

    set(target ${XTTEST_NAME})

    add_executable(${XTTEST_NAME} ${XTTEST_SOURCES})
    # add all flags to the target!
    add_dune_all_flags(${XTTEST_NAME})
    # This is just a placeholder
    target_compile_definitions(${XTTEST_NAME} PUBLIC ${XTTEST_COMPILE_DEFINITIONS})
    target_compile_options(${XTTEST_NAME} PUBLIC ${XTTEST_COMPILE_FLAGS})
    target_link_libraries(${XTTEST_NAME} ${XTTEST_LINK_LIBRARIES})
    set(XTTEST_TARGET ${XTTEST_NAME})


    # Make sure to exclude the target from all, even when it is user-provided
    # This is exactly what dune_add_test does in dune-common
    if(DUNE_BUILD_TESTS_ON_MAKE_ALL)
        set_property(TARGET ${target} PROPERTY EXCLUDE_FROM_ALL 0)
    else()
        set_property(TARGET ${target} PROPERTY EXCLUDE_FROM_ALL 1)
    endif()
    # Have the given target depend on build_tests in order to trigger the build correctly
    if(NOT XTTEST_EXPECT_COMPILE_FAIL)
        add_dependencies(build_tests ${XTTEST_TARGET})
    endif()

    # Process the EXPECT_COMPILE_FAIL option
    if(XTTEST_EXPECT_COMPILE_FAIL)
        set(XTTEST_COMMAND ${CMAKE_COMMAND} --build . --target ${XTTEST_TARGET} --config $<CONFIGURATION>)
    endif()

    # Add one test for each specified processor number
    foreach(procnum ${XTTEST_MPI_RANKS})
        if((NOT "${procnum}" GREATER "${DUNE_MAX_TEST_CORES}") AND (NOT XTTEST_COMPILE_ONLY))
            if(NOT ${procnum} STREQUAL "1")
                set(ACTUAL_NAME "${XTTEST_NAME}-mpi-${procnum}")
            else()
                set(ACTUAL_NAME ${XTTEST_NAME})
            endif()

            if(NOT ${MPI_CXX_FOUND})
                _add_test(NAME ${ACTUAL_NAME}
                        COMMAND ${CMAKE_BINARY_DIR}/dune-env ${XTTEST_SCRIPT}
                        --exec ${target}
                        --ini "NONE"
                        --source ${CMAKE_CURRENT_SOURCE_DIR}
                        )
            else()
                _add_test(NAME ${ACTUAL_NAME}
                        COMMAND ${CMAKE_BINARY_DIR}/dune-env ${XTTEST_SCRIPT}
                        --exec ${target}
                        --ini "NONE"
                        --source ${CMAKE_CURRENT_SOURCE_DIR}
                        --mpi-exec "${MPIEXEC}"
                        --mpi-numprocflag=${MPIEXEC_NUMPROC_FLAG}
                        --mpi-preflags "${MPIEXEC_PREFLAGS}"
                        --mpi-postflags "${MPIEXEC_POSTFLAGS}"
                        --max-processors=${DUNE_MAX_TEST_CORES}
                        )
            endif()

            # Define the number of processors (ctest will coordinate this with the -j option)
            set_tests_properties(${ACTUAL_NAME} PROPERTIES PROCESSORS ${procnum})
            # Apply the timeout (which was defaulted to 5 minutes if not specified)
            set_tests_properties(${ACTUAL_NAME} PROPERTIES TIMEOUT ${XTTEST_TIMEOUT})
            # Process the EXPECT_FAIL option
            if(XTTEST_EXPECT_COMPILE_FAIL OR XTTEST_EXPECT_FAIL)
                set_tests_properties(${ACTUAL_NAME} PROPERTIES WILL_FAIL true)
            endif()
            # Skip the test if the return code is 77!
            set_tests_properties(${ACTUAL_NAME} PROPERTIES SKIP_RETURN_CODE 77)
        endif()
    endforeach()

endfunction()
