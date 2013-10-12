begin
  require "rubysl/un"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-un", e
end
