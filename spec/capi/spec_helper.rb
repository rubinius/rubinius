require File.dirname(__FILE__) + '/../spec_helper'

require 'rbconfig'

def compile_extension(path, ext, ext_source)
  signature = "#{path}compiled_by"

  return if File.exists?(signature) and
            IO.read(signature).chomp == RUBY_NAME and
            File.exists?(ext) and File.mtime(ext) > File.mtime(ext_source)

  # TODO use Tap; write a common ext build task
  if RUBY_NAME == 'rbx'
    `./bin/rbx compile -I#{Rubinius::HDR_PATH} #{ext_source}`
  else
    # TODO add compile commands for MRI
  end

  File.open(signature, "w") { |f| f.puts RUBY_NAME }
end

def load_extension(name)
  path = File.dirname(__FILE__) + '/ext/'
  ext = "#{path}#{name}_spec.#{Config::CONFIG['DLEXT']}"
  ext_source = "#{path}#{name}_spec.c"

  compile_extension path, ext, ext_source
  require ext
end
