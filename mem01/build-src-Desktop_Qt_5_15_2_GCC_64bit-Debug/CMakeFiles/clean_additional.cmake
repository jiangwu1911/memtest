# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/mem01_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/mem01_autogen.dir/ParseCache.txt"
  "mem01_autogen"
  )
endif()
