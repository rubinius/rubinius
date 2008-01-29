require 'compiler/text'

# "Interactive" mode
def interactive()
  require 'readline'

  c = Compiler.new(Compiler::TextGenerator)
  puts "Enter ? for help, ^D to exit."

  while code = Readline.readline("rbx:describe> ")
    if code == "?"
      puts "Enter any valid Ruby expression to see its compilation process."
      next
    end

    code = code.to_sexp

    puts ""
    puts code.indented_inspect
    puts c.into_script(code).to_description.generator.text
    puts ""
  end

  exit
end

require 'pp'

Compiler.parse_flags ARGV

file = ARGV.shift
unless file
  interactive()
  exit 0
end

puts "Sexp:"
pp File.to_sexp(file)

puts "\nBytecode:"

puts Compiler.compile_file(file).decode

