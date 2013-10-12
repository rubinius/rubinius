begin
  require "rubysl/monitor"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-monitor", e
end
