if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  # better cpu detection on Linux (lifted from cmake cvs)
  exec_program(${CMAKE_UNAME} ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

add_subdirectory(shotgun)
