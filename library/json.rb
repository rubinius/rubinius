begin
  require "rubysl/json"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-json", e
end
