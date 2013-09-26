begin
  require "rubysl/openssl"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-openssl", e
end
