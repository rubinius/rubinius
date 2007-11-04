require 'compiler'
require 'generate'
require 'bytecode'
require 'text'

gen = Compiler::TextGenerator

c = Compiler.new(gen)

file = ARGV.shift

if file
  puts "Parsing #{file}"
  n = c.into_script(File.to_sexp(file))
else
  code = Readline.readline("> ")
  n = c.into_script(code.to_sexp)
end

p n
p n.to_description
# puts gen.text
