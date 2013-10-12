begin
  require "rubysl/net/telnet"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-net-telnet", e
end
