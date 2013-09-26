begin
  require "rubysl/complex"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-complex", e
end
