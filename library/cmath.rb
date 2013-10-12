begin
  require "rubysl/cmath"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-cmath", e
end
