def require_files(files)
  files.each do |path|
    require(path)
  end
end

begin
  require 'spec/rake/spectask'
rescue LoadError
  puts "Unable to load spec/rake/spectask, spec tasks are not available"
  no_spec = true
end

# require local rake libs
# doesn't do anything gracefully on load error (yet)
paths = Dir[ File.join(File.dirname(__FILE__), 'rake/*') ]
require_files(paths) unless no_spec

# By default, run all the specs and tests
task :default => :spec

desc "Run all specs and tests."
task :spec do
  Rake::Task['spec:all'].invoke rescue got_error = true
  
  raise "Spec or test failures." if got_error
end unless no_spec

namespace :spec do
  desc "Run all specs and tests."
  task :all do
    Rake::Task['spec:only'].invoke  rescue got_error = true
    Rake::Task['test:all'].invoke   rescue got_error = true
    
    raise "Spec or test failures." if got_error
  end
  
  desc "Run only specs but not any tests."
  task :only => %w(spec:language spec:shotgun spec:library spec:core
                   spec:targets spec:compatibility)

  # desc is automatically done with SpecTask
  GroupSpecTask.new(:language)
  GroupSpecTask.new(:shotgun)
  GroupSpecTask.new(:library)
  GroupSpecTask.new(:core)
  GroupSpecTask.new(:targets)
  GroupSpecTask.new(:compatibility)

  # experimental -- need to adjust exclusions depending on what your testing
  namespace :coverage do
    desc "Generate a coverage report for the library specs."
    GroupCoverageReport.new(:library)

    desc "Generate a coverage report for the core specs."
    GroupCoverageReport.new(:core)
  end
end unless no_spec

desc "Alias for test:all"
task :test => 'test:all'

namespace :test do
  desc "Run all tests."
  task :all do
    Rake::Task['test:core'].invoke    rescue got_error = true
    Rake::Task['test:shotgun'].invoke rescue got_error = true
    
    raise "Test failures." if got_error
  end

  desc "Runs BFTS on Ruby 1.8.*."
  task :bfts do
    system("ruby -Inative -Ibfts/overlay bfts/overlay/tc_all.rb")
  end
  
  desc "Runs part of BFTS under Rubinius natively."
  task :nativebfts => ['test:setup:code_cache'] do
    system("ruby -Ibfts/overlay bfts/overlay/test_stuff.rb")
  end
  
  desc "Run rubinius's 1.8.* tests."
  task :core do
    system("ruby -Ilib test/tc_all.rb")
  end
  
  desc "Run shotgun's core tests."
  task :shotgun => ['test:setup:code_cache'] do
    system("ruby test/tc_all.rb shotgun-tests")
  end

  namespace :setup do
    desc "Prepare the code-cache directory"
    task :code_cache do
      Dir.mkdir "code-cache" unless File.exists?("code-cache")
      FileUtils.rm Dir.glob("code-cache/*")
    end
  end
end

"Rebuild the whole project"
task :build => 'build:all'

namespace :build do
  desc "Rebuild shotgun from scratch"
  task :all => ['build:clean', 'setup:syd', 'build:shotgun', 'build:bk']
  
  desc "Cleanup build files."
  task :clean do
    FileUtils.rm_rf 'code-cache'
    FileUtils.rm Dir.glob('lib/kernel.rb*')
    `cd shotgun;make -e clean;cd ..`
  end
  
  task :setup do
    sh "ruby bin/rcc"
  end
  
  namespace :setup do
    desc "Ensure that the correct version of the sydparse gem is installed."
    task :syd do
      require 'rubygems'
      begin
        require_gem 'sydparse', '>= 1.2.1'
      rescue Gem::LoadError
        puts "\nYour system does not have the required sysparse gem installed...\n"
        Rake::Task['syd'].invoke
        raise "Gem 'sydparse' must be installed from externals/syd-parser/pkg directory. Then re-run 'rake build'."
      end
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
  
  desc "Build shotgun C components."
  task :shotgun => 'build:setup' do
    system("make -e -C shotgun rubinius")
  end
  
  desc "Build the kernel."
  task :kernel => 'build:setup:kernel' do
    puts "Compiling kernel.rb..."
    `bin/obsolete.rcompile lib/kernel.rb`
  end

  desc "Build the kernel."
  task :bk => 'build:kernel'
  
  desc "Build syd-parser."
  task :syd do
    puts "Building externals/syd-parser gem...\n"
    system("cd externals/syd-parser; rake gem")
    puts "\nNow do 'gem install externals/syd-parser/pkg/*.gem' as your gem superuser.\n\n"
  end
  
  desc "Bootstrap the compiler."
  task :compiler do
    files = %w! bytecode/compiler bytecode/assembler bytecode/encoder
      sexp/simple_processor translation/normalize translation/local_scoping
      sexp/composite_processor translation/states sexp/exceptions
      bytecode/primitive_names!

    files.each do |name|
      file = "#{name}.rb"
      dir = File.dirname(file)
      dest_dir = File.join("native", dir)
      path = File.expand_path File.join("lib", file)
      dest = File.join("native", file)
      FileUtils.mkdir_p dest_dir
      FileUtils.symlink path, dest rescue nil
      sh "bin/obsolete.rcompile #{dest}"
    end

    extra = %w!bytecode/rubinius!
    extra.each do |name|
      sh "bin/obsolete.rcompile native/#{name}.rb"
    end
  end
end

namespace :doc do
  desc "Learn how to contribute."
  task :contrib => 'doc:contrib:easy'
  
  namespace :contrib do
    desc "Find out about easy ways to contribute."
    task :easy do
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
    
    desc "Find out about ways to contribute that may require a lot of knowledge or work."
    task :hard do
      puts "More to come. For now, just make everything work faster than anything else in the world."
    end
  end
end

namespace :svk do
  task :push do
    sh 'svk push --verbatim'
  end
end

# Deprecated rake tasks
def deprecate(msg)
  puts "Deprecated. Please use #{msg}."
end

desc "DEPRECATED: Run shotgun's core tests"
task :test_shotgun do
  deprecate 'test:shotgun'
  Rake::Task['test:shotgun'].invoke
end

desc "DEPRECATED: Run all the tests"
task :test_all do
  deprecate 'test:all'
  Rake::Task['test:all'].invoke
end

# This forces ruby inline to build everything in the 
# right place.
task :setup do
  deprecate 'build:setup'
  Rake::Task['build:setup'].invoke
end

desc "DEPRECATED: Build shotgun C components"
task :shotgun do
  deprecate 'build:shotgun'
  Rake::Task['build:shotgun'].invoke
end

desc "DEPRECATED: Prepare the code-cache directory"
task :code_cache do
  deprecate 'test:setup:code-cache'
  Rake::Task['test:setup:code-cache'].invoke
end

desc "DEPRECATED: Ensure that the correct version of the sydparse gem is installed."
task :ensure_syd do
  deprecate 'build:setup:syd'
  Rake::Task['build:setup:syd'].invoke
end  

desc "DEPRECATED: Build syd-parser."
task :syd do
  deprecate 'build:syd'
  Rake::Task['build:syd'].invoke
end

task :fields do
  deprecate 'build:fields'
  Rake::Task['build:fields'].invoke
end

# Combine the separate .rb files in lib into a single kernel.rb
task :kernel do
  deprecate 'build:setup:kernel'
  Rake::Task['build:setup:kernel'].invoke
end

# Build the kernel
task :bk do
  deprecate 'build:bk'
  Rake::Task['build:kernel'].invoke
end

task :clean do
  deprecate 'build:clean'
  Rake::Task['build:clean'].invoke
end

task :push do
  deprecate 'svk:push'
  Rake::Task['svk:push'].invoke
end

desc "DEPRECATED: Alias for doc:contrib:easy."
task :find_low_hanging_fruit do
  deprecate 'doc:contrib:easy'
  Rake::Task['doc:contrib:easy'].invoke
end

# vim: syntax=ruby
