begin
  require "rubysl/yaml"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-yaml", e
end
