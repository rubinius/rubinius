#!shotgun/rubinius

require 'bytecode/compiler'
require 'bytecode/rubinius'

# recognize a few simple options
def usage
  puts <<-USAGE
  Usage: sirb [options]
    sirb is a RCEPL (read, compile, execute, print, loop) program for rubinius

    Options: 
            -p   Print the parsed s-expression
            -x   Print the normalized s-expression.
            -s   Print assembly instructions.
            -b   Print bytecode encoding.
  USAGE
end

  
begin
  until ARGV.empty?
    arg = ARGV.shift
    case arg
    when '-h'
      usage
    when '-p'
      $show_parse = true
    when '-x'
      $show_sexp = true
    when '-s'
      $show_asm = true
    when '-b'
      $show_bytes = true
    else
      puts "Unrecognized option: #{arg}"
    end
  end
end

def print_bytecodes(bc)
  str = ""
  bc.each do |b|
    str << b.chr
  end
  puts "\nBytecodes:", str.inspect
end

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
      puts "\nS-exp:\n#{sexp.inspect}" if $show_parse
      nx = compiler.fully_normalize(sexp)
      puts "\nNormalized S-exp:\n#{nx.inspect}" if $show_sexp
      desc = compiler.compile_as_method(nx, :__eval_script__)
      puts "\nAsm:\n#{desc.assembly}" if $show_asm
      cm = desc.to_cmethod
      print_bytecodes(cm.bytecodes) if $show_bytes
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
