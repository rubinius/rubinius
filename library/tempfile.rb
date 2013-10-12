begin
  require "rubysl/tempfile"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-tempfile", e
end
