set(soversion 2)

CHECK_INCLUDE_FILE(sys/inotify.h HAVE_SYS_INOTIFY_H)
CHECK_INCLUDE_FILE(sys/epoll.h   HAVE_SYS_EPOLL_H)
CHECK_INCLUDE_FILE(sys/event.h   HAVE_SYS_EVENT_H)
CHECK_INCLUDE_FILE(sys/queue.h   HAVE_SYS_QUEUE_H)
CHECK_INCLUDE_FILE(port.h        HAVE_PORT_H)
CHECK_INCLUDE_FILE(poll.h        HAVE_POLL_H)
CHECK_INCLUDE_FILE(sys/select.h  HAVE_SYS_SELECT_H)

CHECK_FUNCTION_EXISTS(inotify_init HAVE_INOTIFY_INIT)
CHECK_FUNCTION_EXISTS(epoll_ctl    HAVE_EPOLL_CTL)
CHECK_FUNCTION_EXISTS(kqueue       HAVE_KQUEUE)
CHECK_FUNCTION_EXISTS(port_create  HAVE_PORT_CREATE)
CHECK_FUNCTION_EXISTS(poll         HAVE_POLL)
CHECK_FUNCTION_EXISTS(select       HAVE_SELECT)

macro(find_function_looking_in_librt func var)
  CHECK_FUNCTION_EXISTS(${func} ${var})
  
  # if not found, try to link librt
  if(NOT ${var})
    if(NOT HAVE_LIBRT)
      CHECK_LIBRARY_EXISTS(rt ${func} "" HAVE_LIBRT)
    endif(NOT HAVE_LIBRT)
    
    if(HAVE_LIBRT)
      set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} rt)
    
      # give it a second chance
      CHECK_FUNCTION_EXISTS(${func} funcfound)
      set(${var} 1 CACHE INTERNAL "Have function ${FUNCTION}" FORCE)
    endif(HAVE_LIBRT)
  endif(NOT ${var})
endmacro(find_function_looking_in_librt)

find_function_looking_in_librt(clock_gettime HAVE_CLOCK_GETTIME)
find_function_looking_in_librt(nanosleep     HAVE_NANOSLEEP)
	
CHECK_LIBRARY_EXISTS(m ceil "" HAVE_LIBM)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake 
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_library(ev SHARED ev.c event.c)
set_target_properties(ev PROPERTIES SOVERSION ${soversion})
