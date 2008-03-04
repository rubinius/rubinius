option(FFI_DEBUG      "Define this if you want extra debugging")
option(FFI_NO_STRUCTS "Define this is you do not want support for aggregate types")
option(FFI_NO_RAW_API "Define this is you do not want support for the raw API")
option(USING_PURIFY   "Define this if you are using Purify and want to suppress spurious messages")


include(platforms.cmake)

# this snippet resembles AC_FUNC_MMAP_BLACKLIST from acinclude.m4
CHECK_INCLUDE_FILE(sys/mman.h HAVE_SYS_MMAN_H)
CHECK_FUNCTION_EXISTS(mmap HAVE_MMAP)

if(HAVE_SYS_MMAN_H AND HAVE_MMAP)
  # this actually should be false for vms & ultrix (see acinclude.m4)
  set(HAVE_MMAP_FILE 1 CACHE INTERNAL "Read-only mmap of a plain file works" FORCE)

  # also doesn't work for vms, ultrix, hpux10*, hpux11.00 (see acinclude.m4)
  if(NOT WIN32 AND NOT DARWIN)
    set(HAVE_MMAP_DEV_ZERO 1 CACHE INTERNAL "Mmap of /dev/zero works" FORCE)
  endif(NOT WIN32 AND NOT DARWIN)

  CHECK_SYMBOL_EXISTS(MAP_ANON "sys/types.h;sys/mman.h;unistd.h" sym_anon1)
  CHECK_SYMBOL_EXISTS(MAP_ANONYMOUS "sys/types.h;sys/mman.h;unistd.h" sym_anon2)

  if(sym_anon1 OR sym_anon2)
    # Systems known to be in this category are Windows, VMS, and SCO Unix (see acinclude.m4)
    if(NOT WIN32)
      set(HAVE_MMAP_ANON 1 CACHE INTERNAL "Mmap with MAP_ANON(YMOUS) works" FORCE)
    endif(NOT WIN32)
  else(sym_anon1 OR sym_anon2)
  endif(sym_anon1 OR sym_anon2)

endif(HAVE_SYS_MMAN_H AND HAVE_MMAP)

CHECK_FUNCTION_EXISTS(memcpy HAVE_MEMCPY)
CHECK_INCLUDE_FILE(alloca.h HAVE_ALLOCA_H)
CHECK_INCLUDE_FILE(memory.h HAVE_MEMORY_H)

CHECK_TYPE_SIZE(double DOUBLE)
CHECK_TYPE_SIZE("long double" LONG_DOUBLE)

# fix for ffi.h.in
if(HAVE_LONG_DOUBLE STREQUAL TRUE)
  set(HAVE_LONG_DOUBLE 1 CACHE INTERNAL "" FORCE)
endif(HAVE_LONG_DOUBLE STREQUAL TRUE)

TEST_BIG_ENDIAN(WORDS_BIGENDIAN)

CHECK_C_SOURCE_COMPILES(  # note quad-escaping here
  "asm (\".cfi_startproc\\\\n\\\\t.cfi_endproc\");int main(){return 0;}" 
  HAVE_AS_CFI_PSEUDO_OP)

# TODO: snippet for SPARC 
# (HAVE_AS_SPARC_UA_PCREL and HAVE_AS_REGISTER_PSEUDO_OP)

macro(ASSEMBLE flags src var_out)
  set(s  "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.c")
  set(as "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.s")
  file(WRITE "${s}" "${src}\n")

  execute_process(
    COMMAND ${CMAKE_C_COMPILER} -S -o ${as} ${s}
    RESULT_VARIABLE res ERROR_QUIET OUTPUT_QUIET)

  if(NOT res)
    set(${var_out} ${as})
  else(NOT res)
    set(${var_out} FALSE)
  endif(NOT res)
endmacro(ASSEMBLE)

find_program(GREP grep)

if(NOT EH_FRAME_FLAGS)
  message(STATUS "Checking whether .eh_frame section should be read-only")

  ASSEMBLE(
    "-fpic -fexceptions"
    "extern void foo (void); void bar (void) { foo (); foo (); }" asm)

  if(asm)
    execute_process(
      COMMAND ${GREP} .section.*eh_frame.*"a" ${asm} 
      RESULT_VARIABLE res OUTPUT_QUIET ERROR_QUIET)

    if(NOT res)
      set(eh_ro_frame 1)
    else(NOT res)
      execute_process(
        COMMAND ${GREP} ".section.*eh_frame.*#alloc" ${asm} 
        COMMAND ${GREP} -v "#write"
        RESULT_VARIABLE res2 OUTPUT_QUIET ERROR_QUIET)

      if(NOT res2)
        set(eh_ro_frame 1)
      endif(NOT res2)
    endif(NOT res)

    if(eh_ro_frame)
      message(STATUS "Checking whether .eh_frame section should be read-only -- it should")
      set(HAVE_RO_EH_FRAME 1 CACHE INTERNAL 
        ".eh_frame sections should be read-only" FORCE)

      set(EH_FRAME_FLAGS   a CACHE INTERNAL 
        "flags needed for the .section .eh_frame directive" FORCE)
    else(eh_ro_frame)
      message(STATUS "Checking whether .eh_frame section should be read-only -- it shouldn't")
      
      set(EH_FRAME_FLAGS  aw CACHE INTERNAL 
        "flags needed for the .section .eh_frame directive" FORCE)
    endif(eh_ro_frame)

    file(REMOVE ${asm})
  else(asm)
    message(STATUS "Checking whether .eh_frame section should be read-only -- failed")
  endif(asm)
endif(NOT EH_FRAME_FLAGS)

if(NOT HIDDEN_TESTED)
  message(STATUS "Checking for __attribute__((visibility(\"hidden\")))")

  ASSEMBLE(
    "-Werror"
    "int __attribute__ ((visibility (\"hidden\"))) foo (void) { return 1; }" asm)

  if(asm)
    execute_process(
      COMMAND ${GREP} \\.hidden.*foo ${asm} 
      RESULT_VARIABLE res OUTPUT_QUIET ERROR_QUIET)
   
    if(NOT res)
      set(HAVE_HIDDEN_VISIBILITY_ATTRIBUTE 1 CACHE INTERNAL 
        "__attribute__((visibility(\"hidden\"))) is supported" FORCE)
  
      message(STATUS "Checking for __attribute__((visibility(\"hidden\"))) -- supported")
    else(NOT res)
      message(STATUS "Checking for __attribute__((visibility(\"hidden\"))) -- unsupported")
    endif(NOT res)

    set(HIDDEN_TESTED 1 CACHE INTERNAL 
        "test for hidden visibility attribute has been done" FORCE)
    
    file(REMOVE ${asm})
  else(asm)
    message(STATUS "Checking for __attribute__((visibility(\"hidden\"))) -- failed")
  endif(asm)
endif(NOT HIDDEN_TESTED)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/fficonfig.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/fficonfig.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/ffi.h.in 
               ${CMAKE_CURRENT_BINARY_DIR}/ffi.h)

set(tsrc ${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGETDIR}/ffitarget.h)
set(tdst ${CMAKE_CURRENT_BINARY_DIR}/ffitarget.h)

if(WIN32)
  set(cmd copy)
else(WIN32)
  set(cmd create_symlink)
endif(WIN32)

execute_process(COMMAND ${CMAKE_COMMAND} -E ${cmd} ${tsrc} ${tdst})

#############################

set(libsrc src/debug.c src/prep_cif.c src/types.c src/raw_api.c src/java_raw_api.c src/closures.c)

macro (ffi_target targ)
  if(${TARGET} STREQUAL ${targ})
    set(libsrc ${libsrc} ${ARGN})
  endif(${TARGET} STREQUAL ${targ})
endmacro (ffi_target)

ffi_target(MIPS            src/mips/ffi.c src/mips/o32.S src/mips/n32.S)
ffi_target(X86             src/x86/ffi.c src/x86/sysv.S)
ffi_target(X86_FREEBSD     src/x86/ffi.c src/x86/freebsd.S)
ffi_target(X86_WIN32       src/x86/ffi.c src/x86/win32.S)
ffi_target(X86_DARWIN      src/x86/ffi.c src/x86/darwin.S src/x86/ffi64.c src/x86/darwin64.S)
ffi_target(SPARC           src/sparc/ffi.c src/sparc/v8.S src/sparc/v9.S)
ffi_target(ALPHA           src/alpha/ffi.c src/alpha/osf.S)
ffi_target(IA64            src/ia64/ffi.c src/ia64/unix.S)
ffi_target(M32R            src/m32r/sysv.S src/m32r/ffi.c)
ffi_target(M68K            src/m68k/ffi.c src/m68k/sysv.S)
ffi_target(POWERPC         src/powerpc/ffi.c src/powerpc/sysv.S src/powerpc/ppc_closure.S 
                           src/powerpc/linux64.S src/powerpc/linux64_closure.S)
ffi_target(POWERPC_AIX     src/powerpc/ffi_darwin.c src/powerpc/aix.S src/powerpc/aix_closure.S)
ffi_target(POWERPC_DARWIN  src/powerpc/ffi_darwin.c src/powerpc/darwin.S src/powerpc/darwin_closure.S)
ffi_target(POWERPC_FREEBSD src/powerpc/ffi.c src/powerpc/sysv.S src/powerpc/ppc_closure.S)
ffi_target(ARM             src/arm/sysv.S src/arm/ffi.c)
ffi_target(LIBFFI_CRIS     src/cris/sysv.S src/cris/ffi.c)
ffi_target(FRV             src/frv/eabi.S src/frv/ffi.c)
ffi_target(S390            src/s390/sysv.S src/s390/ffi.c)
ffi_target(X86_64          src/x86/ffi64.c src/x86/unix64.S src/x86/ffi.c src/x86/sysv.S)
ffi_target(SH              src/sh/sysv.S src/sh/ffi.c)
ffi_target(SH64            src/sh64/sysv.S src/sh64/ffi.c)
ffi_target(PA_LINUX        src/pa/linux.S src/pa/ffi.c)
ffi_target(PA_HPUX         src/pa/hpux32.S src/pa/ffi.c)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# enable .S compiling
set_source_files_properties(${libsrc} PROPERTIES LANGUAGE C)

# for some reason cmake generates invalid rules for .i and .s when .S are given
set(CMAKE_SKIP_PREPROCESSED_SOURCE_RULES 1)
set(CMAKE_SKIP_ASSEMBLY_SOURCE_RULES 1)

add_library(ffi ${libsrc})
