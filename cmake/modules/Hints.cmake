function(APPEND_TO_EACH inputlist postfix outputlist)
  foreach(entry inputlist)
    list(APPEND ${outputlist} ${entry}${postfix})
  endforeach(entry inputlist)
endfunction()

set(hint_prefixes
    "${CMAKE_SOURCE_DIR}/../local/"
    "${CMAKE_SOURCE_DIR}/../environments/debian-minimal/local/"
    "${CMAKE_SOURCE_DIR}/../environments/debian-full/local/"
    "$ENV{HOME}/"
    "$ENV{HOME}/Software/")

set(bin_hints "")
append_to_each(hint_prefixes "bin/" bin_hints)

set(lib_hints "")
append_to_each(hint_prefixes "lib/" lib_hints)

set(include_hints "")
append_to_each(hint_prefixes "include/" include_hints)

