begin
  require "rubysl/optparse"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-optparse", e
end
