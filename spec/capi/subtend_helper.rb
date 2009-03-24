# Compiles the extension used in the subtend specs

require 'rbconfig'

def compile_extension(name)
  path = File.dirname(__FILE__) + '/ext/'

  ext = "#{path}#{name}.#{Config::CONFIG['DLEXT']}"
  ext_source = "#{path}#{name}.c"

  if !File.exists?(ext) or File.mtime(ext) < File.mtime(ext_source)
    system "rake build:debug_flags extension:specs:#{name}"
  end
end
