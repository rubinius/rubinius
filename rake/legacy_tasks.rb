namespace :legacy do
  desc "Ensure that the correct version of the sydparse gem is installed."
  task :check_syd do
    require 'rubygems'
    begin
      gem 'sydparse', '>= 1.2.1'
    rescue Gem::LoadError
      puts "\nYour system does not have the required sysparse gem installed...\n"
      Rake::Task['build_syd'].invoke
      raise "Gem 'sydparse' must be installed from externals/syd-parser/pkg directory."
    end
  end

  desc "Build sydparse gem."
  task :build_syd do
    puts "Building externals/syd-parser gem...\n"
    Dir.chdir('externals/syd-parser')
    system("rake gem")
    raise 'Failed to build externals/syd-parser gem' if $?.exitstatus != 0
    puts "\nNow do 'gem install externals/syd-parser/pkg/*.gem' as your gem superuser.\n\n"
  end

  desc "Build the VM bootstrap archive."
  task :rcompile_bootstrap => :check_syd do
    Dir.chdir "kernel" do
      files = Dir["bootstrap/*.rb"].sort

      changed = []
      files.each do |file|
        cmp = "#{file}c"
        unless newer?(file, cmp)
          changed << cmp
          system "#{COMPILER} #{file}"
          raise "Failed to compile #{file}" if $?.exitstatus != 0
        end
        file << "c"
      end

      File.open(".load_order.txt","w") do |f|
        f.puts files.join("\n")
      end

      archive = "../runtime/bootstrap.rba"

      if File.exists? archive
        if changed.empty?
          puts "No kernel/bootstrap files to update."
        else
          system "zip -u #{archive} .load_order.txt #{changed.join(' ')}"
        end
      else
        system "zip #{archive} .load_order.txt #{files.join(' ')}"
      end
    end
  end

  desc "Build the core classes and methods archive."
  task :rcompile_core => :check_syd do
    files = nil
    Dir.chdir("kernel") do
      files = Dir["core/*.rb"].sort
      files.delete "core/__loader.rb"

      files << "core/__loader.rb"

      changed = []
      files.each do |file|
        cmp = "#{file}c"
        unless newer?(file, cmp) # File.exists?(cmp) and File.mtime(cmp) >= File.mtime(file)
          changed << cmp
          system "#{COMPILER} #{file}"
          raise "Failed to compile #{file}" if $?.exitstatus != 0
        end
        file << "c"
      end

      File.open(".load_order.txt","w") do |f|
        f.puts files.join("\n")
      end

      archive = "../runtime/core.rba"

      if File.exists? archive
        if changed.empty?
          puts "No kernel/core files to update."
        else
          system "zip -u #{archive} .load_order.txt #{changed.join(' ')}"
        end
      else
        system "zip #{archive} .load_order.txt #{files.join(' ')}"
      end
    end
  end
end

