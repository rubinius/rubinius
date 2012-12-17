require File.expand_path('../file_processor', __FILE__)

module Rubinius
module FFI
  class Generator
    def initialize(ffi_name, rb_name)
      STDERR.puts "FFI::Generator is deprecated. Use FFI::FileProcessor"
      STDERR.puts "Called from: #{caller(1).first}"

      FileProcessor.new ffi_name, rb_name
    end
  end
end
end
