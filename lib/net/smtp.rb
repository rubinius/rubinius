begin
  require "rubysl/net/smtp"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-net-smtp", e
end
