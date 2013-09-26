begin
  require "rubysl/xmlrpc"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-xmlrpc", e
end
