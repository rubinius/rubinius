begin
  require "rubysl/date"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-date", e
end
