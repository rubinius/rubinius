begin
  require "rubysl/singleton"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-singleton", e
end
