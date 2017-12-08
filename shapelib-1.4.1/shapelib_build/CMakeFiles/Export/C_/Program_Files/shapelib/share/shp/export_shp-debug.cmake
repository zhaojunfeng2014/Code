#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "shp" for configuration "Debug"
set_property(TARGET shp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(shp PROPERTIES
  IMPORTED_IMPLIB_DEBUG "F:/Code/shapelib-1.4.1/shapelib_build/lib/shp.lib"
  IMPORTED_LOCATION_DEBUG "F:/Code/shapelib-1.4.1/shapelib_build/bin/shp.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS shp )
list(APPEND _IMPORT_CHECK_FILES_FOR_shp "F:/Code/shapelib-1.4.1/shapelib_build/lib/shp.lib" "F:/Code/shapelib-1.4.1/shapelib_build/bin/shp.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
