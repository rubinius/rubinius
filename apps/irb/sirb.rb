#!shotgun/rubinius

require 'bytecode/compiler'
require 'bytecode/rubinius'

context = ""
line = 0
compiler = Bytecode::Compiler.new
MAIN = Object.new

while true
  str = Readline.readline("sirb(eval):#{line}> ")
  context << str
  if context.size > 1
    begin
      line += 1
      sexp = context.to_sexp
      desc = compiler.compile_as_method(sexp, :__eval_script__)
      cm = desc.to_cmethod
      out = cm.activate(MAIN, [])
      puts "=> #{out.inspect}" # do it like this so exit won't do =>
      context = ""
    rescue SyntaxError => e
      # ignore, waiting for a complete expression
    rescue Exception => e
      puts e
      puts e.backtrace.show
      context = ""
    end
  else
    context = ""
    line += 1
  end
end
