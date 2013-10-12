begin
  require "rubysl/prettyprint"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-prettyprint", e
end
