# Compiles the extension used in the subtend specs
require 'rbconfig'
def compile_extension(name)
  path = File.dirname(__FILE__) + '/ext/'
  ext = "#{path}#{name}.#{Config::CONFIG['DLEXT']}"
  ext_source = "#{path}#{name}.c"
  if !File.exists?(ext) or File.mtime(ext) < File.mtime(ext_source)
    if Object.constants.include? 'Rubinius'
      system "./shotgun/rubinius compile #{ext_source} -g > /dev/null"
    else
      c = Config::CONFIG
      system "#{c['LDSHARED']} -g #{ext_source} -I #{c['archdir']} #{c['SOLIBS']} #{c['LIBRUBYARG_SHARED']} -o #{ext} > /dev/null"
    end
  end
end
