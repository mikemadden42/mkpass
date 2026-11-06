# Black-box tests for the mkpass command line.
# Run with: cmake -DMKPASS=/path/to/mkpass -P cli_test.cmake

function(run_mkpass)
    execute_process(
        COMMAND "${MKPASS}" ${ARGN}
        RESULT_VARIABLE rc
        OUTPUT_VARIABLE out
        ERROR_VARIABLE err
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(rc "${rc}" PARENT_SCOPE)
    set(out "${out}" PARENT_SCOPE)
    set(err "${err}" PARENT_SCOPE)
endfunction()

function(expect_password length)
    run_mkpass("${length}")
    if (NOT rc STREQUAL "0")
        message(FATAL_ERROR "length ${length}: exit '${rc}', stderr: ${err}")
    endif()
    string(LENGTH "${out}" actual)
    if (NOT actual EQUAL length)
        message(FATAL_ERROR "length ${length}: got ${actual} characters")
    endif()
    if (NOT out MATCHES "^[0-9A-Za-z]+$")
        message(FATAL_ERROR "length ${length}: bad characters: ${out}")
    endif()
endfunction()

function(expect_rejected)
    run_mkpass(${ARGN})
    if (NOT rc STREQUAL "1")
        message(FATAL_ERROR "args [${ARGN}]: exit '${rc}', expected 1")
    endif()
    if (NOT out STREQUAL "")
        message(FATAL_ERROR "args [${ARGN}]: printed to stdout: ${out}")
    endif()
    if (err STREQUAL "")
        message(FATAL_ERROR "args [${ARGN}]: no error on stderr")
    endif()
endfunction()

# Boundaries. The alphabet has 62 characters, so a password of 63 or more
# only has the right length if characters can repeat.
foreach(length IN ITEMS 12 16 62 63 1024)
    expect_password(${length})
endforeach()

# Out of range
foreach(arg IN ITEMS 0 1 11 1025 99999999999999999999)
    expect_rejected(${arg})
endforeach()

# Not a whole number
foreach(arg IN ITEMS abc 12abc 3.7 -1 -5 +16 0x10)
    expect_rejected(${arg})
endforeach()
expect_rejected("")
expect_rejected(" 16")

# Wrong argument count
expect_rejected()
expect_rejected(16 16)

# Two passwords from separate runs should differ.
run_mkpass(32)
set(first "${out}")
run_mkpass(32)
if (first STREQUAL out)
    message(FATAL_ERROR "two runs produced the same password: ${out}")
endif()
