# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2012 - 2017)
#   Rene Milk       (2010 - 2018)
#   Sven Kaulmann   (2013)
#   Tobias Leibner  (2015 - 2017)
# ~~~

macro(dxt_headercheck_target_name arg)
  string(REGEX REPLACE ".*/([^/]*)" "\\1" simple ${arg})
  string(REPLACE ${PROJECT_SOURCE_DIR} "" rel ${arg})
  string(REGEX REPLACE "(.*)/[^/]*" "\\1" relpath ${rel})
  string(REGEX REPLACE "/" "_" targname ${rel})
  set(targname "headercheck_${targname}")
endmacro(dxt_headercheck_target_name)

macro(get_headercheck_targets)
  file(GLOB_RECURSE bindir_header "${CMAKE_BINARY_DIR}/*.hh")
  list(APPEND dxt_ignore_header ${bindir_header})
  # this is mostly c&p from dune-common, since we need a way to extract all target names to pass to our load balancing
  # script
  if(ENABLE_HEADERCHECK)
    get_property(headerlist GLOBAL PROPERTY headercheck_list)
    foreach(header ${headerlist})
      list(FIND dxt_ignore_header "${header}" _index)
      if(${_index} GREATER -1)
        continue()
      endif() # do some name conversion
      set(targname ${header})
      dxt_headercheck_target_name(${targname})
      list(APPEND dxt_headercheck_targets "${targname}")
    endforeach(header ${headerlist})
  endif(ENABLE_HEADERCHECK)
endmacro(get_headercheck_targets)

macro(BEGIN_TESTCASES)
  # https://cmake.org/cmake/help/v3.0/module/FindGTest.html http://purplekarrot.net/blog/cmake-and-test-suites.html
  file(GLOB_RECURSE test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
  foreach(source ${test_sources})
    set(ranks "1")
    if(source MATCHES "mpi")
      list(APPEND ranks ${DUNE_MAX_TEST_CORES})
    endif(source MATCHES "mpi")
    get_filename_component(testbase ${source} NAME_WE)
    string(REPLACE ".cc" ".mini" minifile ${source})
    if(EXISTS ${minifile})
      dune_add_system_test(SOURCE
                           ${source}
                           ${COMMON_HEADER}
                           INIFILE
                           ${minifile}
                           BASENAME
                           test_${testbase}
                           CREATED_TARGETS
                           targetlist_${testbase}
                           ADDED_TESTS
                           testlist_${testbase}
                           SCRIPT
                           dune_xt_execute.py
                           ${DEBUG_MACRO_TESTS})
      foreach(target ${targetlist_${testbase}})
        target_link_libraries(${target} ${ARGN} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_xt_common)
        list(APPEND dxt_test_binaries ${target})
        set(dxt_test_names_${target} ${testlist_${testbase}_${target}})
      endforeach(target)
    else(EXISTS ${minifile})
      set(target test_${testbase})
      dune_add_test(NAME
                    ${target}
                    SOURCES
                    ${source}
                    ${COMMON_HEADER}
                    LINK_LIBRARIES
                    ${ARGN}
                    ${COMMON_LIBS}
                    ${GRID_LIBS}
                    gtest_dune_xt_common
                    COMMAND
                    ${CMAKE_BINARY_DIR}/dune-env
                    ${CMAKE_CURRENT_BINARY_DIR}/${target}
                    --gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/${target}.xml
                    TIMEOUT
                    ${DXT_TEST_TIMEOUT}
                    MPI_RANKS
                    ${ranks})
      list(APPEND dxt_test_binaries ${target})
      set(dxt_test_names_${target} ${target})
    endif(EXISTS ${minifile})
  endforeach(source)
  file(GLOB_RECURSE test_templates "${CMAKE_CURRENT_SOURCE_DIR}/*.tpl")
  foreach(template ${test_templates})
    set(ranks "1")
    if(template MATCHES "mpi")
      list(APPEND ranks ${DUNE_MAX_TEST_CORES})
    endif(template MATCHES "mpi")
    get_filename_component(testbase ${template} NAME_WE)
    string(REPLACE ".tpl" ".py" config_fn "${template}")
    string(REPLACE ".tpl" ".tpl.cc" out_fn "${template}")
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" out_fn "${out_fn}")
    # get the last completed cache for the codegen execution during configure time
    foreach(_mod ${ALL_DEPENDENCIES})
      dune_module_path(MODULE ${_mod} RESULT ${_mod}_binary_dir BUILD_DIR)
      if(IS_DIRECTORY ${${_mod}_binary_dir})
        set(last_dep_bindir ${${_mod}_binary_dir})
      endif()
    endforeach(_mod DEPENDENCIES)

    dune_xt_execute_process(COMMAND
                            ${CMAKE_BINARY_DIR}/dune-env
                            dxt_code_generation.py
                            "${config_fn}"
                            "${template}"
                            "${CMAKE_CURRENT_BINARY_DIR}"
                            "${out_fn}"
                            "${last_dep_bindir}")
    file(GLOB generated_sources "${out_fn}.*")
    if("" STREQUAL "${generated_sources}")
      set(generated_sources ${out_fn})
    endif()
    add_custom_command(OUTPUT "${generated_sources}"
                       COMMAND ${CMAKE_BINARY_DIR}/dune-env dxt_code_generation.py "${config_fn}" "${template}"
                               "${CMAKE_CURRENT_BINARY_DIR}" "${out_fn}" "${last_dep_bindir}"
                       DEPENDS "${config_fn}" "${template}"
                       VERBATIM USES_TERMINAL)
    foreach(gen_source ${generated_sources})
      string(REPLACE "${out_fn}." "" postfix "${gen_source}")
      string(REPLACE "${out_fn}" "" postfix "${postfix}")
      string(REPLACE ".cc" "" postfix "${postfix}")
      if(NOT "" STREQUAL "${postfix}")
        set(postfix "__${postfix}")
      endif()
      set(target test_${testbase}${postfix})
      dune_add_test(NAME
                    ${target}
                    SOURCES
                    ${gen_source}
                    ${COMMON_HEADER}
                    LINK_LIBRARIES
                    ${ARGN}
                    ${COMMON_LIBS}
                    ${GRID_LIBS}
                    gtest_dune_xt_common
                    COMMAND
                    ${CMAKE_BINARY_DIR}/dune-env
                    ${CMAKE_CURRENT_BINARY_DIR}/${target}
                    --gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/${target}.xml
                    TIMEOUT
                    ${DXT_TEST_TIMEOUT}
                    MPI_RANKS
                    ${ranks})
      list(APPEND dxt_test_binaries ${target})
      set(dxt_test_names_${target} ${target})
    endforeach()
  endforeach(template ${test_templates})
  add_custom_target(test_templates SOURCES ${test_templates})
endmacro(BEGIN_TESTCASES)

macro(END_TESTCASES) # this excludes meta-ini variation test cases because  there binary name != test name
  foreach(test ${dxt_test_binaries})
    if(TARGET test)
      set_tests_properties(${test} PROPERTIES TIMEOUT ${DXT_TEST_TIMEOUT})
    endif(TARGET test)
  endforeach(test ${dxt_test_binaries})

  add_custom_target(test_binaries DEPENDS ${dxt_test_binaries})
  # add_dependencies(test test_binaries)
  add_custom_target(check
                    COMMAND ${CMAKE_CTEST_COMMAND} --timeout ${DXT_TEST_TIMEOUT} -j ${DXT_TEST_PROCS}
                    DEPENDS test_binaries USES_TERMINAL)
  add_custom_target(recheck
                    COMMAND ${CMAKE_CTEST_COMMAND} --timeout ${DXT_TEST_TIMEOUT} --rerun-failed -j ${DXT_TEST_PROCS}
                    DEPENDS test_binaries USES_TERMINAL)
  foreach(target ${dxt_test_binaries})
    set(all_sorted_testnames "${all_sorted_testnames}/${dxt_test_names_${target}}")
  endforeach(target ${dxt_test_binaries})
  set(dxt_headercheck_targets "")
  get_headercheck_targets(dxt_headercheck_targets)
  configure_file(${dune-xt-common-module-path}/dxt_test_binaries.cmake.in
                 ${CMAKE_CURRENT_BINARY_DIR}/dxt_test_binaries.cmake)
  configure_file(${dune-xt-common-module-path}/dxt_all_sorted_testnames.cmake.in
                 ${CMAKE_CURRENT_BINARY_DIR}/dxt_all_sorted_testnames.cmake)
  configure_file(${dune-xt-common-module-path}/dxt_headercheck_targets.cmake.in
                 ${CMAKE_CURRENT_BINARY_DIR}/dxt_headercheck_targets.cmake)
  add_custom_target(rerun_test_distribution
                    ${CMAKE_BINARY_DIR}/dune-env
                    distribute_testing.py
                    "${CMAKE_BINARY_DIR}"
                    "${CMAKE_CURRENT_SOURCE_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}/dxt_test_binaries.cmake"
                    "${CMAKE_CURRENT_BINARY_DIR}/dxt_all_sorted_testnames.cmake"
                    "${CMAKE_CURRENT_BINARY_DIR}/dxt_headercheck_targets.cmake"
                    "${DXT_BIN_COUNT}"
                    VERBATIM USES_TERMINAL)
  add_custom_target(copy_builders_if_different
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/builder_definitions.cmake"
                            "${CMAKE_CURRENT_SOURCE_DIR}/builder_definitions.cmake")
  add_custom_target(refresh_test_timings)
  add_dependencies(copy_builders_if_different rerun_test_distribution)
  add_dependencies(refresh_test_timings copy_builders_if_different)
endmacro(END_TESTCASES)

macro(dxt_exclude_from_headercheck)
  exclude_from_headercheck(${ARGV0}) # make this robust to argument being passed with or without ""
  string(REGEX REPLACE "[\ \n]+([^\ ])" ";\\1" list ${ARGV0})
  set(list "${list};${ARGV}")
  foreach(item ${list})
    set(item ${CMAKE_CURRENT_SOURCE_DIR}/${item})
    list(APPEND dxt_ignore_header ${item})
  endforeach()
endmacro(dxt_exclude_from_headercheck)

macro(dxt_add_python_tests)
  add_custom_target(test_python
                    "${CMAKE_BINARY_DIR}/dune-env"
                    "py.test"
                    "."
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/python"
                    DEPENDS bindings
                    VERBATIM USES_TERMINAL)
endmacro(dxt_add_python_tests)
