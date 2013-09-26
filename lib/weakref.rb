begin
  require "rubysl/weakref"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-weakref", e
end
