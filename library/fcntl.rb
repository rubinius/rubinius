begin
  require "rubysl/fcntl"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-fcntl", e
end
