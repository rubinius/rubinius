unless defined? FFI
  begin
    require "ffi"
  rescue LoadError
    require "rubygems"
    require "ffi"
  end
end

require File.join(File.dirname(__FILE__), 'support', 'ffi_specs')
require File.join(FFISpecs::FIXTURE_DIR, 'classes')
