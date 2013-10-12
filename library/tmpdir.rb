begin
  require "rubysl/tmpdir"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-tmpdir", e
end
