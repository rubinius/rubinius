begin
  require "rubysl/forwardable"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-forwardable", e
end
