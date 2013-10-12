begin
  require "rubysl/gdbm"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-gdbm", e
end
