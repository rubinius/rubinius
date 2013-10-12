begin
  require "rubysl/nkf"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-nkf", e
end
