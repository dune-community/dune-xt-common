macro(append_to_each INPUTLIST POSTFIX OUTPUTLIST)
  foreach(ENTRY ${INPUTLIST})
    list(APPEND ${OUTPUTLIST} ${ENTRY}${POSTFIX})
  endforeach(ENTRY ${INPUTLIST})
endmacro()

set(hint_prefixes
    "${CMAKE_SOURCE_DIR}/../local/"
    "${CMAKE_SOURCE_DIR}/../environments/debian-minimal/local/"
    "${CMAKE_SOURCE_DIR}/../environments/debian-full/local/"
    "$ENV{HOME}/"
    "$ENV{HOME}/Software/")

set(bin_hints "")
append_to_each("${hint_prefixes}" "bin/" bin_hints)

set(lib_hint "")
append_to_each("${hint_prefixes}" "lib/" lib_hints)

set(include_hints "")
append_to_each("${hint_prefixes}" "include/" include_hints)

