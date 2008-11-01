# NOTE! When updating this file, also update INSTALL, if necessary.
# NOTE! Please keep your tasks grouped together.

$VERBOSE = true
$verbose = Rake.application.options.trace || ARGV.delete("-v")

$dlext = Config::CONFIG["DLEXT"]
$compiler = nil

RUBINIUS_BASE = File.expand_path(File.dirname(__FILE__))

$: << "lib"

# require 'tsort'
# require 'rakelib/rubinius'

task :default => %w[build vm:test spec:sydney]

desc "Compile the given ruby file into a .rbc file"
task :compile_ruby, :file do |task, args|
  file = args[:file]
  raise ArgumentError, 'compile_ruby requires a file name' if file.nil?

  rbc = file + 'c'

  compile_ruby file, rbc
end
task :compile_ruby => 'kernel:build' # HACK argument + dependency is broken

desc "Run the given ruby file with the vm"
task :run_ruby, :file do |task, args|
  file = args[:file]
  raise ArgumentError, 'compile_ruby requires a file name' if file.nil?

  rbc = file + 'c'

  compile_ruby file, rbc

  ENV['PROBE'] = 'yes' if $verbose
  ENV['RBX_RUNTIME'] = File.join File.dirname(__FILE__), 'runtime'

  sh 'vm/vm', rbc
end
task :run_ruby => %w[kernel:build vm/vm] # HACK argument + dependency is broken

desc "Compile files with MRI because vm takes forever to do it"
task :precompile => %w[extensions] do
  FileList['{mspec,kernel,lib,spec}/**/*.rb'].each do |rb|
    next unless File.file? rb

    rbc = "#{rb}c"

    if File.exist? rbc then
      rbc_mtime = File.mtime rbc
      next if rbc_mtime >= COMPILER_MTIME or rbc_mtime >= File.mtime(rb)
    end

    mri_compile rb, rbc
  end
end

# See vm.rake for more information
desc "Build everything that needs to be built at default level."
task :build => "build:normal"

desc "Recompile all ruby system files"
task :rebuild => %w[clean build]

desc 'Remove rubinius build files'
task :clean => %w[
  vm:clean
  kernel:clean
  clean:crap
  extension:clean
  configure:clean
]

desc 'Remove rubinius build files and external library build files'
task :distclean => %w[
  clean
  vm:distclean
]

def rbx_compile(from)
  sh "bin/rbx compile -f #{from}"
end

desc 'Compare an rbc file made by MRI with one made by rbx'
task :compare, :file do |task, args|
  file = args[:file]
  raise "Please supply something that exists" unless File.exist? file
  
  mri_compile file, file + 'c.mri'
  rbx_compile file
  
  File.open(file + 'c.mri') do |mri|
    File.open(file + 'c') do |rbx|
      while m = mri.gets and r = rbx.gets
        puts "Line of Failure: #{rbx.lineno}" and break unless m === r
      end
    end
  end
  
end

namespace :clean do
  desc "Cleans up editor files and other misc crap"
  task :crap do
    files = (Dir["*~"] + Dir["**/*~"]).uniq

    rm_f files, :verbose => $verbose unless files.empty?
  end
end
