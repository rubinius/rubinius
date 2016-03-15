provider rubinius {
  /*
     rubinius:::method-entry(classname, methodname, filename, lineno);

     This probe is fired just before a method is entered.

     * `classname` name of the class (a string)
     * `methodname` name of the method about to be executed (a string)
     * `filename` the file name where the method is _being called_ (a string)
     * `lineno` the line number where the method is _being called_ (an int)
  */
  probe method__entry(const char*, const char*, const char*, int);

  /*
     rubinius:::method-return(classname, methodname, filename, lineno);

     This probe is fired just after a method has returned. The arguments are
     the same as "rubinius:::method-entry".
  */
  probe method__return(const char*, const char*, const char*, int);

  /*
     rubinius:::method-native-entry(classname, methodname, filename, lineno);

     This probe is fired just before a native method (C-API) method is entered.
     The arguments are the same as "rubinius:::method-entry".
  */
  probe method__native__entry(const char*, const char*, const char*, int);

  /*
     rubinius:::method-native-return(classname, methodname, filename, lineno);

     This probe is fired just before a native method (C-API) method returns.
     The arguments are the same as "rubinius:::method-entry".
  */
  probe method__native__return(const char*, const char*, const char*, int);

  /*
     rubinius:::method-ffi-entry(classname, methodname, filename, lineno);

     This probe is fired just before a native function (FFI) method is entered.
     The arguments are the same as "rubinius:::method-entry".
  */
  probe method__ffi__entry(const char *, const char *, const char *, int);

  /*
     rubinius:::method-ffi-return(classname, methodname, filename, lineno);

     This probe is fired just before a native function (FFI) method returns.
     The arguments are the same as "rubinius:::method-entry".
  */
  probe method__ffi__return(const char*, const char*, const char*, int);

  /*
     rubinius:::method-primitive-entry(classname, methodname, filename, lineno);

     This probe is fired just before a primitive is entered. The arguments are
     the same as "rubinius:::method-entry".
  */
  probe method__primitive__entry(const char*, const char*, const char*, int);

  /*
     rubinius:::method-primitive-return(classname, methodname, filename, lineno);

     This probe is fired just before a primitive returns. The arguments are
     the same as "rubinius:::method-entry".
  */
  probe method__primitive__return(const char*, const char*, const char*, int);

  /*
     rubinius:::gc-begin(full);

     Fired at the beginning of a GC.
     * `full` whether this is a full GC or not
  */
  probe gc__begin(int full);

  /*
     rubinius:::gc-end(full);

     Fired at the end of a GC.
     * `full` whether this is a full GC or not
  */
  probe gc__end(int full);

  /*
     rubinius:::thread-start(thread_id);

     Fired when a new thread is started
     * `name` the name of the new thread
     * `id` the identifier from the newly started thread
     * `system` flag whether the thread is a system thread
  */
  probe thread__start(const char* name, int id, int system);

  /*
     rubinius:::thread-stop(thread_id);

     Fired when a thread is stopped
     * `name` the name of the new thread
     * `id` the identifier from the newly started thread
     * `system` flag whether the thread is a system thread
  */
  probe thread__stop(const char* name, int id, int system);

  /*
     rubinius:::jit-function-begin(classname, methodname);

     This probe is fired just before a method is jitted.
     * `classname` name of the class (a string)
     * `methodname` name of the method to be jitted
     * `filename` the file name where the method is defined
     * `lineno` the line number where the method is defined
  */
  probe jit__function__begin(const char*, const char*, const char*, int);

  /*
     rubinius:::jit-function-end(classname, methodname);

     This probe is fired just after a method is jitted.
     * `classname` name of the class (a string)
     * `methodname` name of the method that was jitted
     * `filename` the file name where the method is defined
     * `lineno` the line number where the method is defined
  */
  probe jit__function__end(const char*, const char*, const char*, int);

  /*
     rubinius:::ruby-probe(payload, length);

     This probe can be fired from Ruby land.
     * `payload` a const char* buffer with the payload
     * `length` the length of the payload
  */
  probe ruby__probe(const char*, int);

};

#pragma D attributes Stable/Evolving/Common provider rubinius provider
#pragma D attributes Stable/Evolving/Common provider rubinius module
#pragma D attributes Stable/Evolving/Common provider rubinius function
#pragma D attributes Evolving/Evolving/Common provider rubinius name
#pragma D attributes Evolving/Evolving/Common provider rubinius args
