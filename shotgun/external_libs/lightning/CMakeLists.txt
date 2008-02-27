# determine CPU
if(CMAKE_SYSTEM_PROCESSOR MATCHES "i.86")
  set(cpu    i386)
  set(suffix -32)
  set(target I386_32)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  set(cpu    i386)
  set(suffix -64)
  set(target I386_64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "sparc")
  set(cpu    sparc)
  set(target SPARC)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc")
  set(cpu    ppc)
  set(target PPC)
else(CMAKE_SYSTEM_PROCESSOR MATCHES "i.86")
  message(SEND_ERROR "Unsupported CPU: ${CMAKE_SYSTEM_PROCESSOR}")
endif(CMAKE_SYSTEM_PROCESSOR MATCHES "i.86")

message(STATUS "Lightning: using ${cpu}${suffix} as CPU")

# LIGHTNING_CROSS is unset here
set(LIGHTNING_DISASSEMBLE 1 CACHE INTERNAL 
  "Define if the test programs should disassemble the code they produce")

set(_ASM_SAFETY 1 CACHE INTERNAL
  "Define to enable assertions")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake 
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(lightning)

if(NOT target STREQUAL "I386_64")
  # disassembling yet not supported for x86_64
  add_subdirectory(opcode)
endif(NOT target STREQUAL "I386_64")
