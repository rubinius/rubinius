begin
  require "rubysl/mutex_m"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-mutex_m", e
end
