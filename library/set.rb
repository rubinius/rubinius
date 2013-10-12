begin
  require "rubysl/set"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-set", e
end
