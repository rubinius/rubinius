begin
  require "rubysl/socket"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-socket", e
end
