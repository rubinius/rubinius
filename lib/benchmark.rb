begin
  require "rubysl/benchmark"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-benchmark", e
end
