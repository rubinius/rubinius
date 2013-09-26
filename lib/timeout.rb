begin
  require "rubysl/timeout"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-timeout", e
end
