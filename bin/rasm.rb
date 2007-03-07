kind = ARGV.shift
file = ARGV.shift

require 'bytecode/compiler'

if kind == "rubinius"
  require 'bytecode/rubinius'
  sexp = File.to_sexp(file, true)
else
  require 'bytecode/r18'
  require 'sydparse'
  
  io = File.open(file)
  syd = SydneyParser.load_file io
  sexp = syd.sexp(false, true)
  io.close
end

comp = Bytecode::Compiler.new
meth = comp.compile_as_script(sexp, :__script__)

def show_method(m)
  puts m.assembly
  m.literals.each do |lit|
    if Bytecode::MethodDescription === lit
      show_method(lit)
    end
  end 
end

show_method(meth)