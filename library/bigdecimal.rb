begin
  require "rubysl/bigdecimal"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-bigdecimal", e
end
