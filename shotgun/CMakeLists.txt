include(vars.cmake)
message(STATUS "Configuring Rubinius version ${VERSION}")

include(CheckCSourceCompiles)
include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(CheckDIRSymbolExists.cmake)
include(TestBigEndian)
find_package(Ruby)

option(DTRACE "enable DTRACE")

if(WIN32)
  set(GIT cmd.exe /c git)
else(WIN32)
  find_program(GIT git)
endif(WIN32)

if(BUILDREV STREQUAL git)
  execute_process(
    COMMAND ${GIT} rev-list --all -n 1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE res
    OUTPUT_VARIABLE out
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if(NOT res)
    set(BUILDREV ${out})
  endif(NOT res)
endif(BUILDREV STREQUAL git)

message(STATUS "Build revision: ${BUILDREV}")

if(DTRACE)
  message(STATUS "DTRACE enabled")
  set(CONFIG_ENABLE_DTRACE 1)
endif(DTRACE)

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}" RUBY_PLATFORM)

message(STATUS "Ruby platform: ${RUBY_PLATFORM}")

execute_process(
  COMMAND ${RUBY_EXECUTABLE} -e "print Time.now.strftime('%Y-%m-%d')"
  OUTPUT_VARIABLE RELDATE)

message(STATUS "Release date: ${RELDATE}")

set(CODEPATH "${PREFIX}/lib/rubinius/${LIBVER}")
set(RBAPATH  "${PREFIX}/lib/rubinius/${LIBVER}/runtime")
set(EXTPATH  "${PREFIX}/lib/rubinius/${LIBVER}/${RUBY_PLATFORM}")
set(BINPATH  "${PREFIX}/bin")
set(LIBPATH  "${PREFIX}/lib")

TEST_BIG_ENDIAN(BIG_ENDIAN)

CHECK_INCLUDE_FILE(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILE(stddef.h HAVE_STDDEF_H)

CHECK_TYPE_SIZE(uintptr_t WORDSIZE)
math(EXPR WORDSIZE "8*${WORDSIZE}")

if(NOT WORDSIZE EQUAL 32 AND NOT WORDSIZE EQUAL 64)
  message(SEND_ERROR "Word size ${WORDSIZE} is not supported, only 32 and 64 are!")
endif(NOT WORDSIZE EQUAL 32 AND NOT WORDSIZE EQUAL 64)

message(STATUS "This is a ${WORDSIZE}-bit system")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake 
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/environment.rb.cmake 
               ${CMAKE_CURRENT_BINARY_DIR}/environment.rb)

# flags flags flags
add_definitions(-D_GNU_SOURCE)
add_definitions(-Wall -Wwrite-strings)  # warnings
add_definitions(-g -ggdb3)

if(LINUX)
  add_definitions(-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64)
endif(LINUX)

if(DARWIN)
  add_definitions(-D_XOPEN_SOURCE -D_DARWIN_USE_64_BIT_INODE)
endif(DARWIN)

if(NOT MINGW)
  add_definitions(-fPIC)
endif(NOT MINGW)

set(CMAKE_C_FLAGS_PROF  "-O2")
set(CMAKE_C_FLAGS_DEV   "-O0")
set(CMAKE_C_FLAGS_TURBO "-O3 -falign-loops -momit-leaf-frame-pointer -fno-tree-pre")
set(CMAKE_C_FLAGS_REGULAR "-O2 -finline-functions -Winline")

# TODO: this
#ifeq ($(CPU), powerpc)
#  OPTIMIZATIONS+=-falign-loops=16
#endif


# external stuff

add_subdirectory(external_libs/libbstring)
add_subdirectory(external_libs/libcchash)
add_subdirectory(external_libs/libffi)
add_subdirectory(external_libs/libev)
add_subdirectory(external_libs/libltdl)
add_subdirectory(external_libs/libmpa)
add_subdirectory(external_libs/libmquark)
add_subdirectory(external_libs/libtommath)
add_subdirectory(external_libs/libzip)
add_subdirectory(external_libs/onig)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

foreach(d libbstring libcchash libev libmpa libmquark libtommath libzip/lib 
    onig libffi)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external_libs/${d})
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/external_libs/${d})
endforeach(d)

# rbx lib
add_subdirectory(lib)

add_executable(rubinius.bin main.c)
target_link_libraries(rubinius.bin rubinius)

if(FREEBSD)
  target_link_libraries(rubinius.bin crypt pthread)
endif(FREEBSD)

