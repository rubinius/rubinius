project(Rubinius C)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  # better cpu detection on Linux (lifted from cmake cvs)
  exec_program(${CMAKE_UNAME} ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
  set(LINUX 1)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(DARWIN 1)
endif(CMAKE_SYSTEM_NAME MATCHES "Darwin")

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
  set(FREEBSD 1)
endif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")

if(CMAKE_SYSTEM_PROCESSOR STREQUAL x86)
  set(CMAKE_SYSTEM_PROCESSOR i386)
endif(CMAKE_SYSTEM_PROCESSOR STREQUAL x86)

if(WIN32)
  if(MINGW)
    set(CMAKE_SYSTEM_NAME mingw32)
  else(MINGW)
    set(CMAKE_SYSTEM_NAME mswin32)
  endif(MINGW)
  # TODO: cygwin?
endif(WIN32)

add_subdirectory(shotgun)
