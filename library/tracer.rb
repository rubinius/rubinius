begin
  require "rubysl/tracer"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-tracer", e
end
