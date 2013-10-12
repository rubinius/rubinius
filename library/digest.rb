begin
  require "rubysl/digest"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-digest", e
end
