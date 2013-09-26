begin
  require "rubysl/coverage"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-coverage", e
end
