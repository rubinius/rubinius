desc "Rebuild shotgun from scratch"
task :build => [:clean, :ensure_syd, :shotgun, :bk]

desc "Run shotgun's core tests"
task :test_shotgun => [:code_cache] do
  system("ruby test/tc_all.rb shotgun-tests")
end

desc "Run rubinius's 1.8.* tests"
task :test do
  system("ruby -Ilib test/tc_all.rb")
end

desc "Run all the tests"
task :test_all => [:test, :test_shotgun]

# This forces ruby inline to build everything in the 
# right place.
task :setup do
  sh "ruby bin/rcc"
end

desc "Build shotgun C components"
task :shotgun => [:setup] do
  system("make -C shotgun rubinius")
end

desc "Prepare the code-cache directory"
task :code_cache do
  Dir.mkdir "code-cache" unless File.exists?("code-cache")
  FileUtils.rm Dir.glob("code-cache/*")
end

desc "Ensure that the correct version of the sydparse gem is installed."
task :ensure_syd do
  require 'rubygems'
  begin
    require_gem 'sydparse', '>= 1.2.1'
  rescue Gem::LoadError
    puts "\nYour system does not have the required sysparse gem installed...\n"
    Rake::Task['syd'].invoke
    raise "Gem 'sydparse' must be installed from externals/syd-parser/pkg directory. Then re-run 'rake build'."
  end
end  

desc "Build syd-parser."
task :syd do
  puts "Building externals/syd-parser gem...\n"
  system("cd externals/syd-parser; rake gem")
  puts "\nNow do 'gem install externals/syd-parser/pkg/*.gem' as your gem superuser.\n\n"
end

task :fields do
  $:.unshift "lib"
  require 'types'
  fd = File.open("kernel/00auto_fields.rb", "w")
  Rubinius::Types.each do |name, mod|
    next if mod::TotalFields.size == 0
    sname = mod.name.split("::").last
    fd.puts "class #{sname}"
    idx = 0
    str = []
    mod::TotalFields.each do |fel|
      if fel == :instance_variables
        fel = :__ivars__
      end
      str << [":#{fel} => #{idx}"]
      # fd.puts "index_accessor :#{fel}, #{idx}"
      fd.puts "  def #{fel}; Ruby.asm \"push self\\npush #{idx}\\nfetch_field\"; end"
      idx += 1
    end
    fd.puts "  ivar_as_index #{str.join(", ")}"
    fd.puts "end"
  end
  fd.close
end

# Combine the separate .rb files in lib into a single kernel.rb
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

# Build the kernel
task :bk => [:kernel] do
  puts "Compiling kernel.rb..."
    `bin/rcompile lib/kernel.rb`
end

task :clean do
  FileUtils.rm_rf 'code-cache'
  FileUtils.rm Dir.glob('lib/kernel.rb*')
  `cd shotgun;make clean;cd ..`
end

task :push do
  sh 'svk push --verbatim'
end

task :find_low_hanging_fruit do
  puts <<-EOM
  The Rubinius team welcomes contributions, bug reports, test cases, and monetary support.
  One possible way to help is:
  1. Add a test for a Ruby core method.
  2. Go to the appropriately-named file in the 'kernel' directory.
  3. Implement that method in Ruby.
  4. Run the tests until they pass. :)
  The 'ri' command is a rich source of examples and test cases.
  EOM
end

# vim: syntax=ruby
