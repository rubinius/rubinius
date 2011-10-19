file = ARGV.shift
klass = ARGV.shift

require 'tempfile'

path = "llvm-type-temp.cpp"

File.open(path, "w+") do |f|
  f.puts "#include \"#{file}\""
  f.puts "void useme(#{klass}* thing);"
  f.puts "void blah(#{klass}* thing) { useme(thing); }"
end

str = `llvm-g++ -I. -I.. -Ivendor/libtommath -emit-llvm -S -o - "#{path}"`

str.split("\n").each do |line|
  if /%"?struct.#{klass}"? = type/.match(line)
    puts line
  end
end
