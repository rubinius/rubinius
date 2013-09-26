begin
  require "rubysl/logger"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-logger", e
end
