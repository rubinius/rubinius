begin
  require "rubysl/abbrev"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-abbrev", e
end
