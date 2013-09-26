begin
  require "rubysl/syslog"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-syslog", e
end
