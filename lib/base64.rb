begin
  require "rubysl/base64"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-base64", e
end
