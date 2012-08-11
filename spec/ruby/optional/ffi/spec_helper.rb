#
# This file is part of ruby-ffi.
#
# This code is free software: you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3 only, as
# published by the Free Software Foundation.
#
# This code is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
# version 3 for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with this work.  If not, see <http://www.gnu.org/licenses/>.
#
require 'rbconfig'

FFI_RUBY_SIGNATURE = "#{RUBY_NAME}-#{RUBY_VERSION}"

def compile_file(file)
  base      = file.gsub(/\.[^\.]+\Z/, "")
  obj       = "#{base}.o"
  signature = "#{base}.sig"

  return false if File.exists?(signature) and
                  IO.read(signature).chomp == FFI_RUBY_SIGNATURE and
                  File.exists?(obj) and File.mtime(obj) > File.mtime(file)

  cc        = RbConfig::CONFIG["CC"]
  cflags    = (ENV["CFLAGS"] || RbConfig::CONFIG["CFLAGS"]).dup
  output = `#{cc} #{cflags} -c #{file} -o #{obj}`

  if $?.exitstatus != 0 or !File.exists?(obj)
    puts "ERROR:\n#{file}"
    raise "Unable to compile \"#{file}\""
  end

  File.open(signature, "w") { |f| f.puts FFI_RUBY_SIGNATURE }
  true
end

def compile_library(path, lib)

  dir = File.expand_path("../#{path}", __FILE__)
  files = Dir["#{dir}/*.{c,cpp}"]
  needs_compile = false
  files.each do |file|
    needs_compile ||= compile_file(file)
  end

  lib = "#{dir}/#{lib}"
  if !File.exists?(lib) || needs_compile
    ldshared  = RbConfig::CONFIG["LDSHARED"]
    libs      = RbConfig::CONFIG["LIBS"]
    dldflags  = RbConfig::CONFIG["DLDFLAGS"]

    output = `#{ldshared} #{files.join(" ")} #{dldflags} #{libs} -o #{lib}`

    if $?.exitstatus != 0
      puts "ERROR:\n#{output}"
      raise "Unable to link \"#{source}\""
    end
  end

  lib
end

require "ffi"

module TestLibrary
  PATH = compile_library("fixtures", "libtest.#{FFI::Platform::LIBSUFFIX}")
end
module LibTest
  extend FFI::Library
  ffi_lib TestLibrary::PATH
end
