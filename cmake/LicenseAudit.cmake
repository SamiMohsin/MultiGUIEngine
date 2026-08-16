# LicenseAudit.cmake
#
# Scans third_party/ and cross-checks every vendored directory against
# THIRD_PARTY_LICENSES.md. Fails configure if a vendored library has no
# matching entry, per .agent-skills/01_ARCHITECTURE_FLOW.md Section 4.
#
# This is intentionally simple (string search, not a real SPDX parser) —
# it exists to catch "someone vendored a folder and forgot the paperwork",
# not to be a legal compliance tool.

function(mg_run_license_audit)
    set(_tp_dir "${CMAKE_SOURCE_DIR}/third_party")
    set(_license_file "${CMAKE_SOURCE_DIR}/THIRD_PARTY_LICENSES.md")

    if(NOT EXISTS "${_license_file}")
        message(FATAL_ERROR "LicenseAudit: THIRD_PARTY_LICENSES.md is missing.")
    endif()

    if(NOT EXISTS "${_tp_dir}")
        return()
    endif()

    file(GLOB _vendored_dirs LIST_DIRECTORIES true "${_tp_dir}/*")
    file(READ "${_license_file}" _license_contents)

    foreach(_dir ${_vendored_dirs})
        if(IS_DIRECTORY "${_dir}")
            get_filename_component(_name "${_dir}" NAME)
            string(FIND "${_license_contents}" "${_name}" _found)
            if(_found EQUAL -1)
                message(FATAL_ERROR
                    "LicenseAudit: third_party/${_name} is vendored but has "
                    "no entry in THIRD_PARTY_LICENSES.md. Add an entry "
                    "(library, license, SPDX id, justification, consuming "
                    "module) before this can build. See "
                    ".agent-skills/01_ARCHITECTURE_FLOW.md Section 4.")
            endif()
        endif()
    endforeach()

    message(STATUS "LicenseAudit: all vendored third_party/ entries accounted for.")
endfunction()

mg_run_license_audit()
