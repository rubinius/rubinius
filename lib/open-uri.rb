begin
  require "rubysl/open-uri"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-open-uri", e
end
