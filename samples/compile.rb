f = "lib/bytecode/compiler.rb"
puts "Compiling #{f}"
m = Compile.compile_file(f)
p m
puts m.assembly
