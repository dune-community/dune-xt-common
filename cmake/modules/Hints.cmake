FUNCTION(APPEND_TO_EACH inputlist postfix outputlist)
	FOREACH(entry inputlist)
		list(APPEND ${outputlist} ${entry}${postfix})
	ENDFOREACH(entry inputlist)
ENDFUNCTION()

SET(hint_prefixes "${CMAKE_SOURCE_DIR}/../local/mkl/"
                  "${CMAKE_SOURCE_DIR}/../environments/debian-minimal/local/mkl/"
		  "$ENV{HOME}/"
		  "$ENV{HOME}/Software/")
SET(lib_hints "")
APPEND_TO_EACH(hint_prefixes "lib/" lib_hints)

SET(include_hints "")
APPEND_TO_EACH(hint_prefixes "include/" include_hints)

