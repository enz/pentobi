# Helper functions for the set_board tests.
# The including script sets ENGINE and a file name prefix unique among tests.

function(run game commands)
    set(input "${CMAKE_CURRENT_BINARY_DIR}/${prefix}-${game}.gtp")
    file(WRITE "${input}" "${commands}")
    execute_process(
        COMMAND "${ENGINE}" --game ${game} --nobook --level 1 --threads 1 --seed 1 --quiet
        INPUT_FILE "${input}" OUTPUT_VARIABLE output ERROR_VARIABLE error
        RESULT_VARIABLE status TIMEOUT 20)
    file(REMOVE "${input}")
    if(NOT status STREQUAL "0")
        message(FATAL_ERROR "Engine exited with ${status}: ${error}")
    endif()
    set(output "${output}" PARENT_SCOPE)
endfunction()

function(response id marker result)
    string(REGEX MATCH "\n\n[${marker}]${id}( |\n)" header "\n\n${output}")
    if(NOT header)
        message(FATAL_ERROR "Missing ${marker}${id} response")
    endif()
    string(FIND "\n\n${output}" "${header}" start)
    string(LENGTH "\n\n${marker}${id}" prefix_length)
    math(EXPR body_start "${start} + ${prefix_length}")
    string(SUBSTRING "\n\n${output}" ${body_start} -1 tail)
    string(FIND "${tail}" "\n\n" end)
    if(end LESS 0)
        message(FATAL_ERROR "Unterminated response ${id}")
    endif()
    string(SUBSTRING "${tail}" 0 ${end} body)
    string(STRIP "${body}" body)
    set(${result} "${body}" PARENT_SCOPE)
endfunction()

function(check_move legal_id move_id)
    response(${legal_id} "=" legal)
    response(${move_id} "=" move)
    string(FIND "\n${legal}\n" "\n${move}\n" found)
    if(move STREQUAL "" OR found LESS 0)
        message(FATAL_ERROR "Recommendation is not legal: ${move}")
    endif()
endfunction()
