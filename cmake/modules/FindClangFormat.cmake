# Use this module by invoking find_package with the form::                                   
#
#   find_package(ClangFormat
#     [version] # Minimum version e.g. 3.7
#     )                      
#
#   optionally pass the minimal version you require like so find_package(ClangFormat 3.7) 
#   this module set CLANG_FORMAT_EXECUTABLE, CLANG_FORMAT_VERSION
#   and CLANG_FORMAT_FOUND accordingly
#
set(CLANG_FORMAT_FOUND 0)
find_program(format_binary NAMES clang-format clang-format-3.6 clang-format-3.7 clang-format-3.8 clang-format-3.9 clang-format-4.0)
if(EXISTS ${format_binary})
  execute_process(COMMAND ${format_binary} -version OUTPUT_VARIABLE clang_out )
  message("clang verison out" ${clang_out})
  string(REGEX REPLACE ".*clang-format version ([0-9]+\\.[0-9]+).*"  "\\1" CLANG_FORMAT_VERSION ${clang_out})
  set(CLANG_FORMAT_FOUND 1)
  set(CLANG_FORMAT_EXECUTABLE ${format_binary})
  if(optional argument passed)
    check match with CLANG_FORMAT_VERSION
    if(not matched)
      message(INFO "Requested clang-format >= arg_passed, found ${CLANG_FORMAT_VERSION}"
      set(CLANG_FORMAT_FOUND 0)
    endif()
  endif()
  #message("clang-format executable: ${CLANG_FORMAT_EXECUTABLE}")
  #message("clang-format version: ${CLANG_FORMAT_VERSION}")
endif()

