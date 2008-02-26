include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(CheckDIRSymbolExists.cmake)

# external stuff

add_subdirectory(external_libs/libbstring)
add_subdirectory(external_libs/libcchash)
add_subdirectory(external_libs/libev)
add_subdirectory(external_libs/libltdl)
add_subdirectory(external_libs/libmpa)
add_subdirectory(external_libs/libmquark)
add_subdirectory(external_libs/libtommath)
add_subdirectory(external_libs/libzip)
add_subdirectory(external_libs/lightning)
add_subdirectory(external_libs/onig)

# rbx lib
add_subdirectory(lib)

# TODO: more stuff
