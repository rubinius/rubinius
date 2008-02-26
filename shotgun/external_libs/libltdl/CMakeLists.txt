set(soversion 4.0.1)

CHECK_INCLUDE_FILE(argz.h        HAVE_ARGZ_H)
CHECK_INCLUDE_FILE(assert.h      HAVE_ASSERT_H)
CHECK_INCLUDE_FILE(ctype.h       HAVE_CTYPE_H)
CHECK_INCLUDE_FILE(errno.h       HAVE_ERRNO_H)
CHECK_INCLUDE_FILE(malloc.h      HAVE_MALLOC_H)
CHECK_INCLUDE_FILE(memory.h      HAVE_MEMORY_H)
CHECK_INCLUDE_FILE(stdlib.h      HAVE_STDLIB_H)
CHECK_INCLUDE_FILE(stdio.h       HAVE_STDIO_H)
CHECK_INCLUDE_FILE(unistd.h      HAVE_UNISTD_H)
CHECK_INCLUDE_FILE(dl.h          HAVE_DL_H)
CHECK_INCLUDE_FILE(sys/dl.h      HAVE_SYS_DL_H)
CHECK_INCLUDE_FILE(dld.h         HAVE_DLD_H)
CHECK_INCLUDE_FILE(mach-o/dyld.h HAVE_MACH_O_DYLD_H)
CHECK_INCLUDE_FILE(string.h      HAVE_STRING_H)
CHECK_INCLUDE_FILE(strings.h     HAVE_STRINGS_H)
CHECK_INCLUDE_FILE(dlfcn.h       HAVE_DLFCN_H)
CHECK_INCLUDE_FILE(inttypes.h    HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE(stdint.h      HAVE_STDINT_H)
CHECK_INCLUDE_FILE(sys/stat.h    HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE(sys/types.h   HAVE_SYS_TYPES_H)

CHECK_FUNCTION_EXISTS(argz_append     HAVE_ARGZ_APPEND)
CHECK_FUNCTION_EXISTS(argz_create_sep HAVE_ARGZ_CREATE_SEP)
CHECK_FUNCTION_EXISTS(argz_insert     HAVE_ARGZ_INSERT)
CHECK_FUNCTION_EXISTS(argz_next       HAVE_ARGZ_NEXT)
CHECK_FUNCTION_EXISTS(argz_stringify  HAVE_ARGZ_STRINGIFY)
CHECK_FUNCTION_EXISTS(strchr          HAVE_STRCHR)
CHECK_FUNCTION_EXISTS(index           HAVE_INDEX)
CHECK_FUNCTION_EXISTS(strrchr         HAVE_STRRCHR)
CHECK_FUNCTION_EXISTS(rindex          HAVE_RINDEX)
CHECK_FUNCTION_EXISTS(memcpy          HAVE_MEMCPY)
CHECK_FUNCTION_EXISTS(memmove         HAVE_MEMMOVE)
CHECK_FUNCTION_EXISTS(bcopy           HAVE_BCOPY)
CHECK_FUNCTION_EXISTS(strcmp          HAVE_STRCMP)
CHECK_FUNCTION_EXISTS(closedir        HAVE_CLOSEDIR)
CHECK_FUNCTION_EXISTS(opendir         HAVE_OPENDIR)
CHECK_FUNCTION_EXISTS(readdir         HAVE_READDIR)

CHECK_DIRSYMBOL_EXISTS(dirent.h   HAVE_DIRENT_H)
CHECK_DIRSYMBOL_EXISTS(ndir.h     HAVE_NDIR_H)
CHECK_DIRSYMBOL_EXISTS(sys/dir.h  HAVE_SYS_DIR_H)
CHECK_DIRSYMBOL_EXISTS(sys/ndir.h HAVE_SYS_NDIR_H)

# dl stuff
# Look for three exclusive alternatives:
# (dld shl_load; dl dlopen; svdl dlopen)

CHECK_FUNCTION_EXISTS(shl_load dl1)

if(dl1)
  set(HAVE_SHL_LOAD 1 CACHE INTERNAL "Have shl_load function")
else(dl1)
  CHECK_LIBRARY_EXISTS(dld shl_load "" dl2)

  if(dl2)
    set(HAVE_SHL_LOAD 1 CACHE INTERNAL "Have shl_load function")
    set(libadd_dl dld)
  else(dl2)
    CHECK_LIBRARY_EXISTS(dl dlopen "" dl3)

    if(dl3)
      set(HAVE_LIBDL 1 CACHE INTERNAL "Have the libdl library or equivalent")
      set(libadd_dl dl)
      set(dlopen 1)
    else(dl3)
      CHECK_LIBRARY_EXISTS(svld dlopen "" dl4)
      if(dl4)
        set(HAVE_LIBDL 1 CACHE INTERNAL "Have the libdl library or equivalent")
        set(libadd_dl svld)
        set(dlopen 1)
      else(dl4)
        CHECK_LIBRARY_EXISTS(dld dld_link "" dl5)

        if(dl5)
          set(HAVE_DLD 1 CACHE INTERNAL "Have the GNU dld library")
          set(libadd_dl dld)
        else(dl5)
          CHECK_FUNCTION_EXISTS(_dyld_func_lookup HAVE_DYLD)
        endif(dl5)
      endif(dl4)
    endif(dl3)
  endif(dl2)
endif(dl1)

if(dlopen)
  CHECK_LIBRARY_EXISTS(${libadd_dl} dlerror "" HAVE_DLERROR)
endif(dlopen)

SET(CMAKE_EXTRA_INCLUDE_FILES argz.h)
CHECK_TYPE_SIZE(error_t ERROR_T)
SET(CMAKE_EXTRA_INCLUDE_FILES)

if(NOT HAVE_ERROR_T)
  set(error_t int)
endif(NOT HAVE_ERROR_T)

# here comes stuff which we'll hardcode for now

# LTDL_DLOPEN_DEPLIBS gonna be unset for all common platforms

set(LTDL_OBJDIR "." CACHE INTERNAL 
   "Define to the sub-directory in which libtool stores uninstalled libraries")

set(LTDL_SHLIBPATH_VAR LD_LIBRARY_PATH CACHE INTERNAL 
  "Define to the name of the environment variable that determines the dynamic library search path")

set(LTDL_SHLIB_EXT .so CACHE INTERNAL 
  "Define to the extension used for shared libraries, say, .so")

set(LTDL_SYSSEARCHPATH "/usr/local/lib:/lib:/usr/lib" CACHE INTERNAL 
  "Define to the system default library search path")
###################################

# NEED_USCORE - unset
# Define if dlsym() requires a leading underscore in symbol names. */


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake 
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DHAVE_CONFIG_H)

add_library(ltdl SHARED ltdl.c)
set_target_properties(ltdl PROPERTIES SOVERSION ${soversion})
