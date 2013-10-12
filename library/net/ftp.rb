begin
  require "rubysl/net/ftp"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-net-ftp", e
end
