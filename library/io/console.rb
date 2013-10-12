begin
  require "rubysl/io/console"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-io-console", e
end
