file = ARGV.shift

if File.exists?(file)
  puts "Compiling #{file}..."
  compile(file)
else
  puts "Unable to compile '#{file}'"
end
