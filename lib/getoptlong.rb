begin
  require "rubysl/getoptlong"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-getoptlong", e
end
