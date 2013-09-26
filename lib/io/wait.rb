begin
  require "rubysl/io/wait"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-io-wait", e
end
