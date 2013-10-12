begin
  require "rubysl/continuation"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-continuation", e
end
