begin
  require "rubysl/scanf"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-scanf", e
end
