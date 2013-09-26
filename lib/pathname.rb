begin
  require "rubysl/pathname"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-pathname", e
end
