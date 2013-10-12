begin
  require "rubysl/shellwords"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-shellwords", e
end
