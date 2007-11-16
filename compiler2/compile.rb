
file = ARGV.shift

require 'compiler'
require 'generate'
require 'bytecode'
require 'text'

gen = Compiler::Generator

c = Compiler.new(gen)


if file
  puts "Parsing #{file}"
  x = File.to_sexp(file)
  puts x.pretty_inspect
  n = c.into_script(x)
else
  code = Readline.readline("> ")
  n = c.into_script(code.to_sexp)
end

meth = n.to_description

p meth
cm = meth.to_cmethod
p cm

enc = Compiler::Encoder.new
p enc.decode_iseq(cm.bytecodes)

puts "EXECUTING"
cm.compile
cm.activate MAIN, Object, []
# puts meth.generator.text
