# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/HelloQt6Qml_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/HelloQt6Qml_autogen.dir/ParseCache.txt"
  "HelloQt6Qml_autogen"
  )
endif()
