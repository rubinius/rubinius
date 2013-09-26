begin
  require "rubysl/fiber"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-fiber", e
end
