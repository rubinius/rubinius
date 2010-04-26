require File.dirname(__FILE__) + '/../spec_helper'

require 'rbconfig'

def compile_extension(path, name)
  ext       = "#{path}#{name}_spec"
  source    = "#{ext}.c"
  obj       = "#{ext}.o"
  lib       = "#{ext}.#{Config::CONFIG['DLEXT']}"
  signature = "#{ext}.sig"

  # TODO use rakelib/ext_helper.rb?
  if RUBY_NAME == 'rbx'
    hdrdir = Rubinius::HDR_PATH
  elsif RUBY_NAME == 'ruby'
    hdrdir = Config::CONFIG["archdir"]
  else
    raise "Don't know how to build C extensions with #{RUBY_NAME}"
  end

  header = hdrdir + "/ruby.h"

  return lib if File.exists?(signature) and
                IO.read(signature).chomp == RUBY_NAME and
                File.exists?(lib) and File.mtime(lib) > File.mtime(source) and
                File.mtime(lib) > File.mtime(header)

  # avoid problems where compilation failed but previous shlib exists
  File.delete lib if File.exists? lib

  cc        = Config::CONFIG["CC"]
  cflags    = (ENV["CFLAGS"] || Config::CONFIG["CFLAGS"]).dup
  cflags   += " -fPIC" unless cflags.include?("-fPIC")
  incflags  = "-I#{path} -I#{hdrdir}"

  `#{cc} #{incflags} #{cflags} -c #{source} -o #{obj}`

  ldshared  = Config::CONFIG["LDSHARED"]
  libpath   = "-L#{path}"
  libs      = Config::CONFIG["LIBS"]
  dldflags  = Config::CONFIG["DLDFLAGS"]

  `#{ldshared} #{obj} #{libpath} #{dldflags} #{libs} -o #{lib}`

  # we don't need to leave the object file around
  File.delete obj if File.exists? obj

  File.open(signature, "w") { |f| f.puts RUBY_NAME }

  lib
end

def load_extension(name)
  path = File.dirname(__FILE__) + '/ext/'

  ext = compile_extension path, name
  require ext
end
