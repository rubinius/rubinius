
desc "Run rubinius's 1.8.* tests"
task :test do
    system("ruby -Ilib test/tc_all.rb")
end

desc "Run shotgun's core tests"
task :test_shotgun do
    Dir.mkdir "code-cache" unless File.exists?("code-cache")
    system("ruby test/tc_all.rb shotgun-tests")
end

desc "Run all the tests"
task :test_all => [:test, :test_shotgun]

# This forces ruby inline to build everything in the 
# right place.
task :setup do
    `ruby bin/rcc`
end

task :shotgun => [:setup] do
    system("make -C shotgun rubinius")
end


desc "Build syd-parser."
task :syd do
    system("cd externals/syd-parser; rake gem")
    puts
    puts
    puts "Now do 'gem install externals/syd-parser/pkg/*.gem' as your gem superuser."
end

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
  Dir["kernel/*.rb"].sort.each do |path|
    next if File.basename(path) == "__loader.rb"
    puts path
    cur = File.open(path)
    fd << cur.read
    cur.close
    fd << "\n"
  end
  
  Dir["kernel/core/*.rb"].each do |path|
    puts path
    fd << File.read(path)
    fd << "\n"
  end
  
  fd << File.read("kernel/__loader.rb")
  fd.close
end

task :bk => [:kernel] do
    puts "Compiling kernel.rb..."
    `bin/rcompile lib/kernel.rb`
end

task :clean do
    `rm -rf code-cache/*`
    `cd shotgun; make clean`
end

# vim: syntax=ruby
