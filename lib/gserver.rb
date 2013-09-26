begin
  require "rubysl/gserver"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-gserver", e
end
