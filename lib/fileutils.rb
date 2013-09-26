begin
  require "rubysl/fileutils"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-fileutils", e
end
