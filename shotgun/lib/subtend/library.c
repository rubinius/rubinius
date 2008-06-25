#include <string.h>
#include <sys/stat.h>
#include <dlfcn.h>

#include "ltdl.h"

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/strlcpy.h"
#include "shotgun/lib/strlcat.h"
#include "shotgun/lib/subtend/nmc.h"
#include "shotgun/lib/subtend/nmethod.h"
#include "shotgun/lib/symbol.h"

#ifdef _WIN32
#define LIBSUFFIX ".dll"
#else
#ifdef __APPLE_CC__
#define LIBSUFFIX ".bundle"
#else
#define LIBSUFFIX ".so"
#endif
#endif

#ifdef __FreeBSD__
#define dlhandle void*
#define xdlopen(name) dlopen(name, RTLD_NOW | RTLD_GLOBAL)
#define xdlsym dlsym
#define xdlsym2(name) dlsym(RTLD_DEFAULT, name)
#define xdlerror dlerror
#else
#define dlhandle lt_dlhandle
#define xdlopen(name) lt_dlopen(name)
#define xdlsym lt_dlsym
#define xdlsym2(name) lt_dlsym(NULL, name)
#define xdlerror lt_dlerror
#endif

void* subtend_find_symbol(STATE, OBJECT path, OBJECT name) {
  dlhandle lib;
  char *c_path, *c_name, *np;
  void *ep;
  char sys_name[128];

  if(!NIL_P(path)) {
    /* path is a string like 'ext/gzip', we turn that into 'ext/gzip.so'
       or whatever the library suffix is. */
    c_path = rbx_string_as_cstr(state, path);
    strlcpy(sys_name, c_path, sizeof(sys_name));
    strlcat(sys_name, LIBSUFFIX, sizeof(sys_name));
    np = sys_name;
  
  } else {
    np = NULL;
  }
  /* Open it up. If this fails, then we just pretend like
     the library isn't there. */
  lib = xdlopen(np);
  if(!lib) {
    return NULL;
  }
  
  c_name = rbx_string_as_cstr(state, name);
  ep = xdlsym(lib, c_name);
  if(!ep) {
    ep = xdlsym2(c_name);
  }
  return ep;
}

/* Call this function to load in a shared library and call
   it's init function. */

OBJECT subtend_load_library(STATE, cpu c, OBJECT path, OBJECT name) {
  dlhandle lib;
  char *c_path, *c_name;
  void (*ep)(void);
  char init[128] = "Init_";
  char *sys_name;
  int len;
  struct stat sb;
  char *end;
  
  /* Try to make room for 'Init_', the extension, and a null byte */
  len = N2I(string_get_bytes(path)) + 21;

  sys_name = ALLOC_N(char, len);
  
  /* path is a string like 'ext/gzip', we turn that into 'ext/gzip.so'
     or whatever the library suffix is. */
  c_path = rbx_string_as_cstr(state, path);
  strlcpy(sys_name, c_path, len);
  end = strrchr(sys_name, '.');
  
  /* detect if the suffix is already there. */
  if(end == NULL || strcmp(end++, LIBSUFFIX)) {
    strlcat(sys_name, LIBSUFFIX, len);
  }
  
  if(stat(sys_name, &sb) == 1) {
    XFREE(sys_name);
    return I2N(0);
  }
  
  /* Open it up. If this fails, then we just pretend like
     the library isn't there. */
  lib = xdlopen(sys_name);
  if(!lib) {
    printf("Couldnt open '%s': %s\n", sys_name, xdlerror());
    XFREE(sys_name);
    /* No need to raise an exception, it's not there. */
    return I2N(0);
  }
  XFREE(sys_name);

  /* name is like 'gzip', we want 'Init_gzip' */
  c_name = rbx_string_as_cstr(state, name);
  strlcat(init, c_name, sizeof(init));
  
  /* Try and load the init function. */
  ep = (void (*)(void))xdlsym(lib, init);
  if(!ep) {
    /* TODO: raise an exception that the library is missing the function. */
    return I2N(1);
  } else {
    OBJECT meth;

    meth = nmethod_new(state, BASIC_CLASS(object), c_path, init, ep, 0);
    cpu_flush_ip(c);
    cpu_flush_sp(c);
    cpu_save_registers(state, c, 0);

    OBJECT sym_name = symtbl_lookup_cstr(state, state->global->symbols, init);

    OBJECT nmc = nmc_new(state, meth, c->active_context, BASIC_CLASS(object), sym_name, Qnil, 0);
    struct fast_context* fc = FASTCTX(nmc);
    rni_nmc* n = (rni_nmc*)fc->opaque_data;
    n->ignore_return = 1;
    /* Protects the common case where users store things created in
     * Init_ into static variables. */
    n->all_global = 1;

    nmc_activate(state, c, nmc, Qnil, FALSE);
  }

  /*
   * We can't close the library while there are references to the code
   * in it. For now, we just leak the library reference, but we need
   * to track it so we can clean them up at some point in the future.
   *
  */

  return Qtrue;
}
