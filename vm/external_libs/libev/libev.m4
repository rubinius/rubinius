dnl this file is part of libev, do not make local modifications
dnl http://software.schmorp.de/pkg/libev

dnl libev support 
AC_CHECK_HEADERS(sys/inotify.h sys/epoll.h sys/event.h sys/queue.h port.h poll.h sys/select.h sys/eventfd.h) 
 
AC_CHECK_FUNCS(inotify_init epoll_ctl kqueue port_create poll select eventfd)
 
AC_CHECK_FUNC(clock_gettime, [], [ 
   if test -z "$LIBEV_M4_AVOID_LIBRT"; then
      AC_CHECK_LIB(rt, clock_gettime) 
      unset ac_cv_func_clock_gettime
      AC_CHECK_FUNCS(clock_gettime)
   fi
])

AC_CHECK_FUNC(nanosleep, [], [ 
   if test -z "$LIBEV_M4_AVOID_LIBRT"; then
      AC_CHECK_LIB(rt, nanosleep) 
      unset ac_cv_func_nanosleep
      AC_CHECK_FUNCS(nanosleep)
   fi
])

AC_CHECK_LIB(m, ceil)



