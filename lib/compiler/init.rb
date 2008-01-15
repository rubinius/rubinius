require 'compiler/compiler'

# When the compiler is compiled into an rba, use the modtime of
# compiler.rba as the version number
if defined? RUBY_ENGINE and RUBY_ENGINE == "rbx"
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

Compile.register_compiler Compiler
