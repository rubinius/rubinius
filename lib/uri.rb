begin
  require "rubysl/uri"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-uri", e
end
