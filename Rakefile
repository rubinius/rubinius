
task :fields do
  $:.unshift "lib"
  require 'types'
  fd = File.open("kernel/auto_fields.rb", "w")
  Rubinius::Types.each do |name, mod|
    next if mod::TotalFields.size == 0
    sname = mod.name.split("::").last
    fd.puts "class #{sname}"
    idx = 0
    mod::TotalFields.each do |fel|
      # fd.puts "index_accessor :#{fel}, #{idx}"
      fd.puts "def #{fel}; Ruby.asm \"push self\\npush #{idx}\\nfetch_field\"; end"
      idx += 1
    end
    fd.puts "end"
  end
  fd.close
end

task :kernel do
  fd = File.open("lib/kernel.rb", "w")
  Dir["kernel/*.rb"].each do |path|
    puts path
    cur = File.open(path)
    fd << cur.read
    cur.close
    fd << "\n"
  end
  fd.close
end

# vim: syntax=ruby
