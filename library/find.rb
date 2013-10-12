begin
  require "rubysl/find"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-find", e
end
