begin
  require "rubysl/pstore"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-pstore", e
end
