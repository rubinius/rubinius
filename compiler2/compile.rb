
file = ARGV.shift

require 'compiler2/compiler'
require 'compiler2/generate'
require 'compiler2/bytecode'
require 'compiler2/text'

gen = Compiler::Generator

c = Compiler.new(gen)


if file
  puts "Parsing #{file}"
  x = File.to_sexp(file)
  puts x.pretty_inspect
  n = c.into_script(x)
else
  require 'readline'
  code = Readline.readline("> ")
  n = c.into_script(code.to_sexp)
end

meth = n.to_description

cm = meth.to_cmethod

enc = Compiler::Encoder.new
p enc.decode_iseq(cm.bytecodes)

puts "EXECUTING"
# cm.compile
cm.activate MAIN, Object, []
# puts meth.generator.text
