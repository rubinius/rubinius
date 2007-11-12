
file = ARGV.shift

require 'compiler'
require 'generate'
require 'bytecode'
require 'text'

gen = Compiler::TextGenerator

c = Compiler.new(gen)


if file
  puts "Parsing #{file}"
  n = c.into_script(File.to_sexp(file))
else
  code = Readline.readline("> ")
  n = c.into_script(code.to_sexp)
end

meth = n.to_description

# puts meth.generator.text
