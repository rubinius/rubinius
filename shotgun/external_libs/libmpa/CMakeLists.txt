set(soversion 0.1)

add_library(ptr_array SHARED ptr_array.c)
set_target_properties(ptr_array PROPERTIES SOVERSION ${soversion})

