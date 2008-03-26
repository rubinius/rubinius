
if ARGV[0].prefix? "-I"
  extra = ARGV.shift[2..-1].split(":")
  extra.each { |n| $:.unshift n }
end

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

puts "File: #{file}"

puts "Sexp:"
pp File.to_sexp(file)

puts "\nBytecode:"

top = Compiler.compile_file(file)

puts top.decode

extra = top.literals.to_a.find_all { |l| l.kind_of? CompiledMethod }

until extra.empty?
  cm = extra.shift
  puts "= #{cm.name} (0x#{cm.object_id.to_s(16)}) ======================"
  puts cm.decode
  extra += cm.literals.to_a.find_all { |l| l.kind_of? CompiledMethod }
end

