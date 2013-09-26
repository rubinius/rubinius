begin
  require "rubysl/securerandom"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-securerandom", e
end
