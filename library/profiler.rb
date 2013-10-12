begin
  require "rubysl/profiler"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-profiler", e
end
