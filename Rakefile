# NOTE! When updating this file, also update INSTALL, if necessary.
# NOTE! Please keep your tasks grouped together.

$VERBOSE = true
$verbose = Rake.application.options.trace
$dlext = Config::CONFIG["DLEXT"]
$compiler = nil

RUBINIUS_BASE = File.expand_path(File.dirname(__FILE__))

# require 'tsort'
# require 'rakelib/rubinius'
# require 'rakelib/struct_generator'
# require 'rakelib/const_generator'
# require 'rakelib/types_generator'

task :default => %w[build vm:test]

# BUILD TASKS

# task :stable_compiler do
#   if ENV['USE_CURRENT'] or ENV['SYSTEM']
#     puts "Use current versions, not stable."
#   else
#     ENV['RBX_BOOTSTRAP'] = "runtime/stable/bootstrap.rba"
#     ENV['RBX_CORE'] = "runtime/stable/core.rba"
#     ENV['RBX_LOADER'] = "runtime/stable/loader.rbc"
#     ENV['RBX_PLATFORM'] = "runtime/stable/platform.rba"
#   end
# end

# files = FileList['kernel/core/*.rb']

# unless files.include?("kernel/core/dir.rb")
#   files.add("kernel/core/dir.rb")
# end

# Core      = CodeGroup.new(files, 'runtime/core', 'core')

# Bootstrap = CodeGroup.new 'kernel/bootstrap/*.rb', 'runtime/bootstrap',
#                           'bootstrap'
# PlatformFiles  = CodeGroup.new 'kernel/platform/*.rb', 'runtime/platform', 'platform'

# file 'runtime/loader.rbc' => 'kernel/loader.rb' do
#   compile 'kernel/loader.rb', 'runtime/loader.rbc'
# end

# file 'runtime/stable/loader.rbc' => 'runtime/loader.rbc' do
#   cp 'runtime/loader.rbc', 'runtime/stable', :verbose => $verbose
# end

# file 'runtime/stable/compiler.rba' => 'build:compiler' do |t|
#   #sh "cd lib; zip -r ../runtime/stable/compiler.rba compiler -x \\*.rb"
#   rm_f t.name
#   rbc_files = Rake::FileList['compiler/**/*.rbc']

#   Dir.chdir 'lib' do
#     rbc_files.each do |rbc_file|
#       ar_add "../#{t.name}", rbc_file
#     end
#   end
# end

# Rake::StructGeneratorTask.new do |t|
#   t.dest = "lib/etc.rb"
# end

# Rake::StructGeneratorTask.new do |t|
#   t.dest = 'lib/zlib.rb'
# end

# AllPreCompiled = Core.output + Bootstrap.output + PlatformFiles.output
# AllPreCompiled << "runtime/loader.rbc"

desc "Build everything that needs to be built"
task :build => %w[
  vm
  kernel:build
  runtime/platform.conf
]
#  build:rbc
#  compiler
#  lib/etc.rb
#  lib/rbconfig.rb
#  extensions

namespace :build do

#   task :setup_rbc => :stable_compiler

#   task :rbc => ([:setup_rbc] + AllPreCompiled)

#   task :compiler => :stable_compiler do
#     compile_dir "lib/compiler"
#   end

#   desc "Rebuild runtime/stable/*.  If you don't know why you're running this, don't."
#   task :stable => %w[
#     build:all
#     runtime/stable/bootstrap.rba
#     runtime/stable/core.rba
#     runtime/stable/compiler.rba
#     runtime/stable/loader.rbc
#     runtime/stable/platform.rba
#   ]

#   desc "Rebuild the .load_order.txt files"
#   task "load_order" do
#     # Note: Steps to rebuild load_order were defined above
#   end

#   namespace :vm do
#     task "clean" do
#       sh "cd shotgun/lib; #{make "clean"}"
#     end

#     task "dev" do
#       sh "cd shotgun/lib; #{make "DEV=1"}"
#     end
#   end

end

# # INSTALL TASKS

# desc "Install rubinius as rbx"
# task :install => :config_env do
#   sh "cd shotgun; #{make "install"}"

#   mkdir_p ENV['RBAPATH'], :verbose => true
#   mkdir_p ENV['CODEPATH'], :verbose => true

#   rba_files = Rake::FileList.new('runtime/platform.conf',
#                                  'runtime/**/*.rb{a,c}',
#                                  'runtime/**/.load_order.txt')

#   install_files rba_files, ENV['RBAPATH']

#   lib_files = Rake::FileList.new 'lib/**/*'

#   install_files lib_files, ENV['CODEPATH']

#   mkdir_p File.join(ENV['CODEPATH'], 'bin'), :verbose => true

#   Rake::FileList.new("#{ENV['CODEPATH']}/**/*.rb").sort.each do |rb_file|
#     sh File.join(ENV['BINPATH'], 'rbx'), 'compile', rb_file, :verbose => true
#   end
# end

# desc "Uninstall rubinius and libraries. Helps with build problems."
# task :uninstall => :config_env do
#   rm Dir[File.join(ENV['BINPATH'], 'rbx*')]
#   rm_r Dir[File.join(ENV['LIBPATH'], '*rubinius*')]
# end

# task :config_env => 'shotgun/config.mk' do
#   File.foreach 'shotgun/config.mk' do |line|
#     next unless line =~ /(.*?)=(.*)/
#     ENV[$1] = $2
#   end
# end

# task :compiledir => :stable_compiler do
#   dir = ENV['DIR']
#   raise "Use DIR= to set which directory" if !dir or dir.empty?
#   compile_dir(dir)
# end

# # CLEAN TASKS

desc "Recompile all ruby system files"
task :rebuild => %w[clean build]

desc 'Remove rubinius build files'
task :clean => %w[vm:clean kernel:clean clean:crap]

desc 'Remove rubinius build files and external library build files'
task :distclean => %w[vm:distclean]

namespace :clean do
#   desc "Cleans all compiled extension files (lib/ext)"
#   task :extensions do
#     Dir["lib/ext/**/*#{$dlext}"].each do |f|
#       rm_f f, :verbose => $verbose
#     end
#   end

#   desc "Cleans up generated files"
#   task :generated do
#     rm_f Dir["shotgun/lib/grammar.c"], :verbose => $verbose
#   end

  desc "Cleans up editor files and other misc crap"
  task :crap do
    files = (Dir["*~"] + Dir["**/*~"]).uniq

    rm_f files, :verbose => $verbose unless files.empty?
  end
end

# # SPEC TASKS
# desc "Run all 'known good' specs (task alias for spec:ci)"
# task :spec => 'spec:ci'

# namespace :spec do
#   namespace :setup do
#     # Setup for 'Subtend' specs. No need to call this yourself.
#     task :subtend do
#       Dir["spec/subtend/**/Rakefile"].each do |rakefile|
#         sh "rake -f #{rakefile}"
#       end
#     end
#   end

#   desc "Run continuous integration examples"
#   task :ci => :build do
#     clear_compiler

#     target = ENV['SPEC_TARGET'] || 'rbx'
#     system %(shotgun/rubinius -e 'puts "rbx build: \#{Rubinius::BUILDREV}"') if target == 'rbx'
#     sh "bin/mspec ci -t #{target}"
#   end

#   spec_targets = %w(compiler core language library parser rubinius)
#   # Build a spec:<task_name> for each group of Rubinius specs
#   spec_targets.each do |group|
#     desc "Run #{group} examples"
#     task group do
#       sh "bin/mspec spec/#{group}"
#     end
#   end

#   desc "Run subtend (Rubinius C API) examples"
#   task :subtend => "spec:setup:subtend" do
#     sh "bin/mspec spec/rubinius/subtend"
#   end

#   # Specdiffs to make it easier to see what your changes have affected :)
#   desc 'Run specs and produce a diff against current base'
#   task :diff => 'diff:run'

#   namespace :diff do
#     desc 'Run specs and produce a diff against current base'
#     task :run do
#       system 'bin/mspec -f ci -o spec/reports/specdiff.txt spec'
#       system 'diff -u spec/reports/base.txt spec/reports/specdiff.txt'
#       system 'rm spec/reports/specdiff.txt'
#     end

#     desc 'Replace the base spec file with a new one'
#     task :replace do
#       system 'bin/mspec -f ci -o spec/reports/base.txt spec'
#     end
#   end

#   task :r2r do
#     puts ARGV.inspect
#   end
# end

# # MISC TASKS

# desc "Build task for CruiseControl"
# task :ccrb => [:build, 'spec:ci']

