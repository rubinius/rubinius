begin
  require "rubysl/profile"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-profile", e
end
