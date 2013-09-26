begin
  require "rubysl/cgi"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-cgi", e
end
