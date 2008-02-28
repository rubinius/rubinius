#set(soversion 0.1)

include_directories(../libcchash ../libmpa)
add_library(mquark quark.c)
#set_target_properties(mquark PROPERTIES SOVERSION ${soversion})
