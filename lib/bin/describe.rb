require 'compiler1/bytecode/compiler'
require 'compiler1/bytecode/rubinius'

file = ARGV.shift

sexp = if File.exist? file then
         File.to_sexp file
       else
         file.to_sexp
       end
puts "\nSexp:\n  #{sexp.inspect}"

compiler = Compiler1::Bytecode::Compiler.new
state = Compiler1::RsLocalState.new

begin
  desc = compiler.compile_as_script(sexp, :__eval_script__, state)
rescue Object => e
  puts "Unable to compile."
  raise e
end
puts "\nAssembly:\n#{desc.assembly}"

puts "Sub methods:"

def show_method(parent)
  parent.literals.each do |lit|
    if Compiler1::Bytecode::MethodDescription === lit
      puts "\n[[ #{parent.name} >> #{lit.name} ]]"
      puts lit.assembly
      show_method(lit)
    end
  end
end

show_method(desc)
