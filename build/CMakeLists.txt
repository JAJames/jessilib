# Include any compiler-specifc settings
if (EXISTS "${JESSILIB_ROOT}/build/${CMAKE_CXX_COMPILER_ID}")
    include("${JESSILIB_ROOT}/build/${CMAKE_CXX_COMPILER_ID}/CMakeLists.txt")
else()
    message(STATUS "No compiler-specific settings set; CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
endif()
