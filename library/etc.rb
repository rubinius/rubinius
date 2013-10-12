begin
  require "rubysl/etc"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-etc", e
end
