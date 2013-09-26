begin
  require "rubysl/net/imap"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-net-imap", e
end
