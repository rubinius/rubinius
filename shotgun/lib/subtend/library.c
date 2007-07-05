#include "shotgun.h"
#include "nmc.h"

#include <string.h>

#include "ltdl.h"

#include "string.h"
#include "strlcpy.h"
#include "strlcat.h"

#ifdef _WIN32
#define LIBSUFFIX ".dll"
#else
#ifdef __APPLE_CC__
#define LIBSUFFIX ".bundle"
#else
#define LIBSUFFIX ".so"
#endif
#endif

void* subtend_find_symbol(STATE, OBJECT path, OBJECT name) {
  lt_dlhandle lib;
  char *c_path, *c_name, *np;
  void *ep;
  char sys_name[128];

  if(!NIL_P(path)) {
    /* path is a string like 'ext/gzip', we turn that into 'ext/gzip.so'
       or whatever the library suffix is. */
    c_path = string_as_string(state, path);
    strlcpy(sys_name, c_path, sizeof(sys_name));
    strlcat(sys_name, LIBSUFFIX, sizeof(sys_name));
    np = sys_name;
  } else {
    np = NULL;
    c_path = NULL;
  }
  /* Open it up. If this fails, then we just pretend like
     the library isn't there. */
  lib = lt_dlopen(np);
  if(!lib) {
    if(c_path) free(c_path);
    return NULL;
  }
  
  c_name = string_as_string(state, name);

  ep = lt_dlsym(lib, c_name);
  
  if(c_path) free(c_path);
  free(c_name);
  
  return ep;
}

/* Call this function to load in a shared library and call
   it's init function. */

OBJECT subtend_load_library(STATE, cpu c, OBJECT path, OBJECT name) {
  lt_dlhandle lib;
  char *c_path, *c_name;
  void (*ep)(void);
  char init[128] = "Init_";
  char sys_name[128];
  rni_nmc *nmc;
  OBJECT ret = Qtrue;
  
  nmc = NULL;
  
  /* path is a string like 'ext/gzip', we turn that into 'ext/gzip.so'
     or whatever the library suffix is. */
  c_path = string_as_string(state, path);
  strlcpy(sys_name, c_path, sizeof(sys_name));
  strlcat(sys_name, LIBSUFFIX, sizeof(sys_name));
  
  /* Open it up. If this fails, then we just pretend like
     the library isn't there. */
  lib = lt_dlopen(sys_name);
  if(!lib) {
/*    printf("Couldnt open '%s': %s\n", sys_name, lt_dlerror());
*/    free(c_path);
    /* No need to raise an exception, it's not there. */
    return I2N(0);
  }
  
  /* name is like 'gzip', we want 'Init_gzip' */
  c_name = string_as_string(state, name);
  strlcat(init, c_name, sizeof(init));
  
  /* Try and load the init function. */
  ep = (void (*)(void))lt_dlsym(lib, init);
  if(!ep) {
    /* TODO: raise an exception that the library is missing the function. */
    ret = I2N(1);
  } else {
    nmc = nmc_new_standalone();
    
    /* Now we need to setup the 'global' context so that stuff like
       rb_define_method works. */
    subtend_set_context(state, c, nmc);

    /* Now perform the call. */
    (*ep)();
  }
   
  /*
   * We can't close the library while there are references to the code
   * in it. For now, we just leak the library reference, but we need
   * to track it so we can clean them up at some point in the future.
   * 
  */
  
  if(nmc) free(nmc);
  free(c_path);
  free(c_name);
  
  subtend_set_context(state, c, NULL);
  
  return ret;
}
