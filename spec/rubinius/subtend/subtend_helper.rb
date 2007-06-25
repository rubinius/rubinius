# Compiles the extension used in the subtend specs
def compile_extension(name)
  path = 'spec/rubinius/subtend/ext/'
  ext = "#{path}#{name}.#{Rubinius::LIBSUFFIX}"
  ext_source = "#{path}#{name}.c"
  if !File.exists?(ext) or File.mtime(ext) < File.mtime(ext_source)
    system "./shotgun/rubinius compile #{ext_source} -g > /dev/null"
  end
end
