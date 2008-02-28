#set(version 1)
add_library(cchash hashtable.c hashtable_itr.c hashtable_utility.c)
#set_target_properties(cchash PROPERTIES SOVERSION ${version})

