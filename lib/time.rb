begin
  require "rubysl/time"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-time", e
end
