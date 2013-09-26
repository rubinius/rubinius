begin
  require "rubysl/ipaddr"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-ipaddr", e
end
