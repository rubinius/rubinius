require 'bytecode/compiler'
require 'bytecode/rubinius'

file = ARGV.shift

fd = File.open(file)
code = fd.read
fd.close

puts "Path: #{file}"
puts "Size: #{code.size} bytes"

compiler = Bytecode::Compiler.new
state = RsLocalState.new

sexp = code.to_sexp
puts "\nSexp:\n  #{sexp.pretty_inspect}"
nx = compiler.fully_normalize(sexp, state)
puts "\nNormalized Sexp:\n  #{nx.pretty_inspect}"
desc = compiler.compile_as_method(nx, :__eval_script__, state)
puts "\nAssembly:\n#{desc.assembly}"
