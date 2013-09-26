begin
  require "rubysl/zlib"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-zlib", e
end
