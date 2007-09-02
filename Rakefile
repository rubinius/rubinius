task :default => :tasks

task :tasks do
  puts `rake -T`
end

def newer?(file, cmp)
  File.exists?(cmp) and File.mtime(cmp) >= File.mtime(file)
end

@pb = "runtime/pristine_bootstrap.rba"
@pp = "runtime/pristine_platform.rba"
@pc = "runtime/pristine_core.rba"
@pl = "runtime/pristine_loader.rbc"

if File.exists?(@pb)
  puts "Using #{@pb} for bootstrap."
  ENV['BOOTSTRAP'] = @pb
end

if File.exists?(@pc)
  puts "Using #{@pc} for core."
  ENV['CORE'] = @pc
end

if File.exists?(@pl)
  puts "Using #{@pl} for the loader."
  ENV['LOADER'] = @pl
end

if File.exists?(@pp)
  puts "Using #{@pp} for the platform."
  ENV['PLATFORM'] = @pp
end

@compiler = ENV['COMPILER']

def update_archive(files, archive, dir=nil)
  archive = File.expand_path(ENV['OUTPUT'] || archive)

  changed = []
  files.each do |file|
    cmp = "#{file}c"
    if !newer?(file, cmp)
      changed << cmp
      if @compiler
        system "shotgun/rubinius -I#{@compiler} compile #{file}"
      else
        system "shotgun/rubinius compile #{file}"
      end
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
desc "Run all 'known good' specs (task alias for spec:ci)"
task :spec => 'spec:ci'

namespace :spec do
  namespace :setup do
    # Setup for 'Subtend' specs. No need to call this yourself.
    task :subtend do
      Dir[File.join(ROOT,"spec/subtend/**/Rakefile")].each do |rakefile|
        sh "rake -f #{rakefile}"
      end
    end
  end

  desc "Run continuous integration examples"
  task :ci do
    target = ENV['SPEC_TARGET'] || 'rbx'
    system %(shotgun/rubinius -e 'puts "rbx build: \#{Rubinius::BUILDREV}"') if target == 'rbx'
    sh "bin/ci -t #{target}"
  end

  spec_targets = %w(compiler core language library parser rubinius)
  # Build a spec:<task_name> for each group of Rubinius specs
  spec_targets.each do |group|
    desc "Run #{group} examples"
    task group do
      sh "bin/mspec spec/#{group}"
    end
  end

  desc "Run subtend (Rubinius C API) examples"
  task :subtend => "spec:setup:subtend" do
    sh "bin/mspec spec/rubinius/subtend"
  end

  # Specdiffs to make it easier to see what your changes have affected :)
  desc 'Run specs and produce a diff against current base'
  task :diff => 'diff:run'

  namespace :diff do
    desc 'Run specs and produce a diff against current base'
    task :run do
      system 'bin/mspec -f ci -o spec/reports/specdiff.txt spec' 
      system 'diff -u spec/reports/base.txt spec/reports/specdiff.txt'
      system 'rm spec/reports/specdiff.txt'
    end

    desc 'Replace the base spec file with a new one'
    task :replace do
      system 'bin/mspec -f ci -o spec/reports/base.txt spec' 
    end
  end

  task :r2r do
    puts ARGV.inspect
  end
end

desc "Build Shotgun (task alias for build:shotgun)"
task :build => ['build:shotgun']

desc "Removes build by-products for Rubinius runtime components"
task :clean => ['clean:compiler', 'clean:bootstrap', 'clean:core', 'clean:library']

namespace :clean do
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

namespace :build do
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
    update_archive files, 'runtime/core.rba'
  end

  task :loader do
    i = "kernel/loader.rb"
    o = ENV['OUTPUT'] || "runtime/loader.rbc"

    if @compiler
      system "shotgun/rubinius -I#{@compiler} compile #{i} #{o}"
    else
      system "shotgun/rubinius compile #{i} #{o}"
    end
  end

  desc "Compiles the Rubinius library archive"
  task :library do
    files = Dir["library/**/*.rb"].sort
    update_archive files, 'runtime/library.rba'
  end

  desc "Compiles the Rubinius compiler archive"
  task :compiler do
    files = Dir["compiler/**/*.rb"].sort   
    update_archive files, 'runtime/compiler.rba', "compiler"
  end

  desc "Compiles the Rubinius platform archive"
  task :platform do
    files = Dir["kernel/platform/*.rb"].sort   
    update_archive files, 'runtime/platform.rba'
  end
end

desc "Remove all compiled Ruby files"
task :pristine do
  FileList['**/*.rbc'].each do |fn|
    next if fn == 'runtime/loader.rbc'
    FileUtils.rm fn rescue nil
  end
end

# dev tasks
namespace :dev do
  desc "Make a snapshot of the runtime files for your own safety"
  task :setup do
    sh "cp runtime/core.rba #{@pc}"
    sh "cp runtime/bootstrap.rba #{@pb}" 
    sh "cp runtime/platform.rba #{@pp}" 
    sh "cp runtime/loader.rbc #{@pl}" 
  end
end

