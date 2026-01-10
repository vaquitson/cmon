#----------------------------------------------------------------
# Generated CMake target import file for configuration "RELWITHDEBINFO".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "llhttp::llhttp_shared" for configuration "RELWITHDEBINFO"
set_property(TARGET llhttp::llhttp_shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(llhttp::llhttp_shared PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libllhttp.so.8.1.0"
  IMPORTED_SONAME_RELWITHDEBINFO "libllhttp.so.8.1"
  )

list(APPEND _cmake_import_check_targets llhttp::llhttp_shared )
list(APPEND _cmake_import_check_files_for_llhttp::llhttp_shared "${_IMPORT_PREFIX}/lib/libllhttp.so.8.1.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
