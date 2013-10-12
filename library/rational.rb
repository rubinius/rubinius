begin
  require "rubysl/rational"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-rational", e
end
