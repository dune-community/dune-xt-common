function(APPEND_TO_EACH inputlist postfix outputlist)
  foreach(entry inputlist)
    list(APPEND ${outputlist} ${entry}${postfix})
  endforeach(entry inputlist)
endfunction()

set(hint_prefixes
    "${CMAKE_SOURCE_DIR}/../local/mkl/"
    "${CMAKE_SOURCE_DIR}/../environments/debian-minimal/local/mkl/"
    "$ENV{HOME}/"
    "$ENV{HOME}/Software/")
set(lib_hints "")
append_to_each(hint_prefixes "lib/" lib_hints)

set(include_hints "")
append_to_each(hint_prefixes "include/" include_hints)

