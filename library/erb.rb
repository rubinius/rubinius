begin
  require "rubysl/erb"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-erb", e
end
