begin
  require "rubysl/english"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-english", e
end
