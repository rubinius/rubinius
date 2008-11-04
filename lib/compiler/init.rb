require 'compiler/compiler'

# When the compiler is compiled into an rba, use the modtime of
# compiler.rba as the version number
if false # defined? RUBY_ENGINE and RUBY_ENGINE == "rbx"
  hints = __METHOD__.hints
  if hints and hints[:source] == :rba
    if path = $:.detect { |m| m.suffix? "compiler.rba" }
      ver = File.mtime(path).to_i
    else
      ver = nil
    end

    Compiler.version_number = ver
  end
end

Compile.compiler = Compiler

# Mask that we just loaded a compiler by removing the constant
# and removing it's files from the feature list.
# $".delete_if { |n| n.prefix? "compiler/" }
# Object.const_set 'Compiler', nil
