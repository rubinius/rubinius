begin
  require "rubysl/net/http"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-net-http", e
end
