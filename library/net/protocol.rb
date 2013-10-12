begin
  require "rubysl/net/protocol"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-net-protocol", e
end
