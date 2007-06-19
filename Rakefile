require 'rake/legacy_tasks'
require 'rake/rubinius_spec_tasks'

task :default => :tasks

task :tasks do
  puts `rake -T`
end

def newer?(file, cmp)
  File.exists?(cmp) and File.mtime(cmp) >= File.mtime(file)
end

def update_archive(files, archive, dir=nil)
  archive = File.expand_path(ENV['OUTPUT'] || archive)
  
  changed = []
  files.each do |file|
    cmp = "#{file}c"
    if !newer?(file, cmp)
      changed << cmp
      system "shotgun/rubinius compile #{file}"
    elsif !File.exists?(archive)
      changed << cmp
    end
    file << "c"
  end
  
  curdir = Dir.getwd
  if dir
    Dir.chdir(dir)    
    changed.map! { |f| f.gsub(%r!^#{dir}/!, "") }
  end
  
  File.open(".load_order.txt","w") do |f|
    f.puts files.join("\n")
  end
  
  if File.exists? archive
    if changed.empty?
      puts "No files to update."
    else
      system "zip -u #{archive} .load_order.txt #{changed.join(' ')}"
    end
  else
    system "zip #{archive} .load_order.txt #{changed.join(' ')}"
  end
  
  Dir.chdir(curdir) if dir
end

# spec tasks
namespace :spec do
  namespace :setup do
    desc "Setup for subtend examples"
    task :subtend do
      Dir[File.join(ROOT,"spec/subtend/**/Rakefile")].each do |rakefile|
        sh "rake -f #{rakefile}"
      end
    end
  end
  
  desc "Run continuous integration examples"
  task :ci do
    begin
      stat = File.stat("CI-specs")
      if stat.file?
        raise "CI-specs is a file. Remove it to run CI specs"
      elsif stat.directory?
        sh "svn up CI-specs"
      end
    rescue
      sh("svn co http://code.fallingsnow.net/svn/rubinius/branches/CI-specs")
    end
    sh "bin/mspec CI-specs"
  end

  spec_targets = %w(compiler core incompatible language library parser rubinius)
  # Build a spec:<task_name> for each group of Rubinius specs
  spec_targets.each do |group|
    desc "Run #{group} examples"
    task group do
      sh "bin/mspec spec/#{group}"
    end
  end

  desc "Run subtend examples"
  task :subtend => "spec:setup:subtend" do
    sh "bin/mspec spec/subtend"
  end

  # Specdiffs to make it easier to see what your changes have affected :)
  desc 'Run specs and produce a diff against current base'
  task :diff => 'diff:run'

  namespace :diff do
    desc 'Run specs and produce a diff against current base'
    task :run do
      system 'bin/mspec spec > /tmp/rbs_specdiff' 
      system 'diff -u spec/diffs/base.txt /tmp/rbs_specdiff'
      system 'rm /tmp/rbs_specdiff'
    end

    desc 'Replace the base spec file with a new one'
    task :replace do
      system 'bin/mspec spec > spec/diffs/base.txt' 
    end
  end

  task :r2r do
    puts ARGV.inspect
  end
end

# build tasks
desc "Completely rebuild everything"
task :rebuild => ['build:clean', 'build:shotgun', 'build:compiler', 'build:bootstrap', 'build:core', 'build:library']

desc "Build shotgun (the C-code VM)"
task :build => ['build:shotgun']

namespace :build do
  
  desc "Removes build by-products for shotgun, compiler, and library"
  task :clean => ['clean:shotgun', 'clean:compiler', 'clean:bootstrap', 'clean:core', 'clean:library']
  
  namespace :clean do
    desc "Removes build by-products for shotgun"
    task :shotgun do
      sh "make -C shotgun clean || true"
    end
    
    desc "Removes build by-products for compiler"
    task :compiler do
      FileList['native/**/*.rbc', '/tmp/*.rbc'].each do |fn|
        FileUtils.rm fn rescue nil
      end
    end
    
    desc "Removes build by-products for library"
    task :library do
      FileList['library/**/*.rbc'].each do |fn|
        FileUtils.rm fn rescue nil
      end
    end

    desc "Removes build by-products for bootstrap"
    task :bootstrap do
      FileList['kernel/bootstrap/**/*.rbc'].each do |fn|
        FileUtils.rm fn rescue nil
      end
    end

    desc "Removes build by-products for core"
    task :core do
      FileList['kernel/core/**/*.rbc'].each do |fn|
        FileUtils.rm fn rescue nil
      end
    end
  end

  file "shotgun/config.h" do
    sh "./configure"
    raise 'Failed to configure Rubinius' unless $?.success?
  end
  
  task :configure => ["shotgun/config.h"] do
  end
  
  desc "Compiles shotgun (the C-code VM)"
  task :shotgun => :configure do
    sh "make"
    raise 'Failed to build shotgun' unless $?.success?
  end
  
  desc "Compiles the Rubinius bootstrap archive"
  task :bootstrap do
    files = Dir["kernel/bootstrap/*.rb"].sort
    update_archive files, 'runtime/bootstrap.rba'
  end

  desc "Compiles the Rubinius core archive"
  task :core do
    files = Dir["kernel/core/*.rb"].sort
    files.delete("kernel/core/__loader.rb")
    files << "kernel/core/__loader.rb"
    update_archive files, 'runtime/core.rba'
  end

  desc "Compiles the Rubinius library archive"
  task :library do
    files = Dir["library/**/*.rb"].sort
    update_archive files, 'runtime/library.rba'
  end

  desc "Compiles the Rubinius compiler archive"
  task :compiler do
    files = %w! bytecode/compiler bytecode/assembler bytecode/encoder
      sexp/simple_processor translation/normalize translation/local_scoping
      sexp/composite_processor translation/states sexp/exceptions
      bytecode/primitive_names !

    paths = []

    files.each do |name|
      file = "#{name}.rb"
      dir = File.dirname(file)
      dest_dir = File.join("native", dir)
      path = File.expand_path File.join("lib", file)
      dest = File.join("native", file)
      FileUtils.mkdir_p dest_dir
      FileUtils.symlink path, dest rescue nil
      
      paths << dest
    end

    paths += %w!native/bytecode/rubinius.rb native/bytecode/system_hints.rb!
   
    update_archive paths, 'runtime/compiler.rba', "native"
  end
end

desc "Remove all .rbc files from the project"
task :pristine do
  FileList['**/*.rbc'].each do |fn|
    FileUtils.rm fn rescue nil
  end
end

# svn tasks
desc "Remove runtime/*.rba then svn up"
task :svn => 'svn:up'
namespace :svn do
  desc "Revert runtime/*.rba then svn up"
  task :up do
    sh "svn revert reports/*.html"
    sh "svn revert runtime/*.rba"
    puts `svn up`
  end
end

