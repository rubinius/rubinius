begin
  require "rubysl/mathn"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-mathn", e
end
