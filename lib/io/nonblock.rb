begin
  require "rubysl/io/nonblock"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-io-nonblock", e
end
