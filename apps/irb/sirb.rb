require 'bytecode/compiler'

str = Readline.readline("irb> ")
while str
  if str
    begin
      sx = str.to_sexp("(irb)", 1, false)
      puts "=== INPUT SEXP"
      p sx
      comp = Bytecode::Compiler.new
      ful = comp.fully_normalize(sx)
      puts "=== NORMALIZED SEXP"
      p ful
      meth = comp.compile_as_script(ful, :__test__)
      puts "=== ASSEMBLY"
      puts meth.assembly
    rescue SyntaxError => e
      puts "SyntaxError: #{e.message}"
      puts  "||  #{e.line}"
      print "||  "
      e.column.times { print " " }
      puts "^"
    end
  end
  
  str = Readline.readline("irb> ")
end