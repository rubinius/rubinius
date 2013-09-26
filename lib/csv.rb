begin
  require "rubysl/csv"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-csv", e
end
