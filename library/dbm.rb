begin
  require "rubysl/dbm"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-dbm", e
end
