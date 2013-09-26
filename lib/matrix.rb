begin
  require "rubysl/matrix"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-matrix", e
end
