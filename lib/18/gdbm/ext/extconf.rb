require 'mkmf'

dir_config("gdbm")
find_header("gdbm.h", "/usr/local/include", "/opt/local/include")
find_library("gdbm", "gdbm_open", "/usr/local/lib", "/opt/local/lib")
if have_library("gdbm", "gdbm_open") and
   have_header("gdbm.h")
  create_makefile("gdbm")
end
