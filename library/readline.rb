begin
  require "rubysl/readline"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-readline", e
end
