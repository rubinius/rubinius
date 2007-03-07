Dir["kernel/bootstrap/*.rb"].each do |file|
  puts file
  system("ruby bin/compare_assembly.rb #{file}")
end

Dir["kernel/core/*.rb"].each do |file|
  puts file
  system("ruby bin/compare_assembly.rb #{file}")
end
