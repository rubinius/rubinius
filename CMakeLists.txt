if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  # better cpu detection on Linux (lifted from cmake cvs)
  exec_program(${CMAKE_UNAME} ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
  set(LINUX 1)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(DARWIN 1)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

add_subdirectory(shotgun)
