begin
  require "rubysl/sdbm"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-sdbm", e
end
