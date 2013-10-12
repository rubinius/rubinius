begin
  require "rubysl/observer"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-observer", e
end
