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
  require 'pp'
  # pp sexp
  io.close
end
#
#def indent_print(array, indent = 0)
#  print "#{' ' * indent}[" 
#
#  array.each_with_index {|elem, i|
#    if elem.kind_of? Array
#      indent_print elem, indent + 2
#      print ",\n"
#      next
#    end
#
#    if i == 0 
#      print ' '
#    else
#      print '  '
#      print ' ' * indent
#    end  
#
#    print elem.inspect
#    print ",\n"
#  }
#
##  print "\n"
#  print "#{' ' * indent}]"
#end
#
#indent_print sexp
comp = Bytecode::Compiler.new
comp.load_hints "kernel/hints"
meth = comp.compile_as_script(sexp, :__script__)

def show_method(m)
  puts "\n[[ #{m.name} ]]"
  puts m.assembly
  p m.locals
  m.literals.each do |lit|
    if Bytecode::MethodDescription === lit
      show_method(lit)
    end
  end
  
  asm = Bytecode::Assembler.new(m.literals, m.name)
  stream = asm.assemble m.assembly
  stream.each { |s| print "  "; p s }
  
end

show_method(meth)
