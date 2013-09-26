begin
  require "rubysl/rss"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-rss", e
end
