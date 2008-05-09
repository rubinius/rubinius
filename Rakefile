# NOTE! When updating this file, also update INSTALL, if necessary.
# NOTE! Please keep your tasks grouped together.

$VERBOSE = true
$verbose = Rake.application.options.trace
$dlext = Config::CONFIG["DLEXT"]
$compiler = nil

RUBINIUS_BASE = File.dirname(__FILE__)

require 'tsort'
require 'rakelib/rubinius'
require 'rakelib/struct_generator'
require 'rakelib/const_generator'
require 'rakelib/types_generator'

task :default => :build

# BUILD TASKS

desc "Build everything that needs to be built"
task :build => 'build:all'

task :stable_compiler do
  if ENV['USE_CURRENT']
    puts "Use current versions, not stable."
  else
    ENV['RBX_BOOTSTRAP'] = "runtime/stable/bootstrap.rba"
    ENV['RBX_CORE'] = "runtime/stable/core.rba"
    ENV['RBX_LOADER'] = "runtime/stable/loader.rbc"
    ENV['RBX_PLATFORM'] = "runtime/stable/platform.rba"
  end
end

rule ".rbc" => %w[.rb] do |t|
  compile t.source, t.name
end

files = FileList['kernel/core/*.rb']

unless files.include?("kernel/core/dir.rb")
  files.add("kernel/core/dir.rb")
end

Core      = CodeGroup.new(files, 'runtime/core', 'core')

Bootstrap = CodeGroup.new 'kernel/bootstrap/*.rb', 'runtime/bootstrap',
                          'bootstrap'
PlatformFiles  = CodeGroup.new 'kernel/platform/*.rb', 'runtime/platform', 'platform'

file 'runtime/loader.rbc' => 'kernel/loader.rb' do
  compile 'kernel/loader.rb', 'runtime/loader.rbc'
end

file 'runtime/stable/loader.rbc' => 'runtime/loader.rbc' do
  cp 'runtime/loader.rbc', 'runtime/stable', :verbose => $verbose
end

file 'runtime/stable/compiler.rba' => 'build:compiler' do |t|
  #sh "cd lib; zip -r ../runtime/stable/compiler.rba compiler -x \\*.rb"
  rm_f t.name
  rbc_files = Rake::FileList['compiler/**/*.rbc']

  Dir.chdir 'lib' do
    rbc_files.each do |rbc_file|
      ar_add "../#{t.name}", rbc_file
    end
  end
end

Rake::StructGeneratorTask.new do |t|
  t.dest = "lib/etc.rb"
end

Rake::StructGeneratorTask.new do |t|
  t.dest = 'lib/zlib.rb'
end

AllPreCompiled = Core.output + Bootstrap.output + PlatformFiles.output
AllPreCompiled << "runtime/loader.rbc"

namespace :build do

  task :all => %w[
    spec:init
    spec:update
    build:shotgun
    build:platform
    build:rbc
    compiler
    lib/etc.rb
    lib/rbconfig.rb
    extensions
  ]

  # This nobody rule lets use use all the shotgun files as
  # prereqs. This rule is run for all those prereqs and just
  # (obviously) does nothing, but it makes rake happy.
  rule '^shotgun/.+'

  # These files must be excluded from the c_source FileList
  # because they are build products (i.e. there is no rule
  # to build them, they will be built) and the c_source list
  # list gets created before they are deleted by the clean task.
  exclude_source = [
    /auto/,
    /instruction_names/,
    /node_types/,
    /grammar.c/,
    'primitive_indexes.h',
    'primitive_util.h'
  ]

  c_source = FileList[
    "shotgun/config.h",
    "shotgun/lib/*.[chy]",
    "shotgun/lib/*.rb",
    "shotgun/lib/subtend/*.[chS]",
    "shotgun/main.c",
  ].exclude(*exclude_source)

  file "shotgun/rubinius.bin" => c_source do
    sh make('vm')
  end
  
  file "shotgun/rubinius.local.bin" => c_source do
    sh make('vm')
  end

  desc "Compiles shotgun (the C-code VM)"
  task :shotgun => %w[configure shotgun/rubinius.bin shotgun/rubinius.local.bin]

  task :setup_rbc => :stable_compiler

  task :rbc => ([:setup_rbc] + AllPreCompiled)

  task :compiler => :stable_compiler do
    compile_dir "lib/compiler"
  end

  desc "Rebuild runtime/stable/*.  If you don't know why you're running this, don't."
  task :stable => %w[
    build:all
    runtime/stable/bootstrap.rba
    runtime/stable/core.rba
    runtime/stable/compiler.rba
    runtime/stable/loader.rbc
    runtime/stable/platform.rba
  ]

  desc "Rebuild the .load_order.txt files"
  task "load_order" do
    # Note: Steps to rebuild load_order were defined above
  end

  namespace :vm do
    task "clean" do
      sh "cd shotgun/lib; #{make "clean"}"
    end

    task "dev" do
      sh "cd shotgun/lib; #{make "DEV=1"}"
    end
  end

  task :platform => 'runtime/platform.conf'
end

# INSTALL TASKS

desc "Install rubinius as rbx"
task :install => :config_env do
  sh "cd shotgun; #{make "install"}"

  mkdir_p RBAPATH, :verbose => true
  mkdir_p CODEPATH, :verbose => true

  rba_files = Rake::FileList.new('runtime/platform.conf',
                                 'runtime/**/*.rb{a,c}',
                                 'runtime/**/.load_order.txt')

  install_files rba_files, RBAPATH

  lib_files = Rake::FileList.new 'lib/**/*'

  install_files lib_files, CODEPATH

  mkdir_p File.join(CODEPATH, 'bin'), :verbose => true

  Rake::FileList.new("#{CODEPATH}/**/*.rb").sort.each do |rb_file|
    if File.exists? "#{rb_file}c"
      next if File.mtime("#{rb_file}c") > File.mtime(rb_file)
    end
    sh File.join(BINPATH, 'rbx'), 'compile', rb_file, :verbose => true
  end
end

desc "Uninstall rubinius and libraries. Helps with build problems."
task :uninstall => :config_env do
  rm Dir[File.join(BINPATH, 'rbx*')]
  rm_r Dir[File.join(LIBPATH, '*rubinius*')]
end

task :compiledir => :stable_compiler do
  dir = ENV['DIR']
  raise "Use DIR= to set which directory" if !dir or dir.empty?
  compile_dir(dir)
end

# CLEAN TASKS

desc "Recompile all ruby system files"
task :rebuild => %w[clean build:all]

desc "Alias for clean:all"
task :clean => "clean:all"

desc "Alias for clean:distclean"
task :distclean => "clean:distclean"

namespace :clean do
  desc "Clean everything but third-party libs"
  task :all => %w[clean:rbc clean:extensions clean:shotgun clean:generated clean:crap clean:config]

  desc "Clean everything including third-party libs"
  task :distclean => %w[clean:all clean:external]

  desc "Remove all compile system ruby files"
  task :rbc do
    files_to_delete = []
    files_to_delete += Dir["*.rbc"] + Dir["**/*.rbc"] + Dir["**/.*.rbc"]
    files_to_delete += Dir["**/.load_order.txt"]
    files_to_delete += ["runtime/platform.conf"]
    files_to_delete -= ["runtime/stable/loader.rbc"] # never ever delete this

    files_to_delete.each do |f|
      rm_f f, :verbose => $verbose
    end
  end

  desc "Cleans all compiled extension files (lib/ext)"
  task :extensions do
    Dir["lib/ext/**/*#{$dlext}"].each do |f|
      rm_f f, :verbose => $verbose
    end
  end

  desc "Cleans up VM building site"
  task :shotgun do
    sh make('clean')
  end

  desc "Cleans up generated files"
  task :generated do
    rm_f Dir["shotgun/lib/grammar.c"], :verbose => $verbose
  end

  desc "Cleans up VM and external libs"
  task :external do
    sh "cd shotgun; #{make('distclean')}"
  end

  desc "Cleans up editor files and other misc crap"
  task :crap do
    rm_f Dir["*~"] + Dir["**/*~"], :verbose => $verbose
  end
  
  desc "Cleans up config files (so they can be regenerated when you change PREFIX)"
  task :config do
    rm "shotgun/config.h", :verbose => $verbose
    rm "shotgun/config.mk", :verbose => $verbose
  end
end

# SPEC TASKS
desc "Run all 'known good' specs (task alias for spec:ci)"
task :spec => 'spec:ci'

namespace :spec do
  namespace :setup do
    # Setup for 'Subtend' specs. No need to call this yourself.
    task :subtend do
      Dir["spec/subtend/**/Rakefile"].each do |rakefile|
        sh "rake -f #{rakefile}"
      end
    end
  end

  desc "Initialize git submodules for mspec and rubyspec"
  task :init => 'mspec:init'

  desc "Update submodule sources for mspec and rubyspec"
  task :update => 'mspec:update'

  task :pull => :update

  desc "Run continuous integration examples"
  task :ci => :build do
    clear_compiler
    sh "bin/mspec ci -t #{spec_target}"
  end

  desc "Run continuous integration examples including stdlib"
  task :full => :build do
    clear_compiler
    sh "bin/mspec ci -t #{spec_target} -B full.mspec"
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

  desc "Run the rspec specs for mspec"
  task :mspec do
    # Use the rspec spec runner (see mspec/README; gem install rspec)
    sh 'spec ./mspec/spec'
  end

  task :r2r do
    puts ARGV.inspect
  end
end

namespace :mspec do
  desc "Initialize git submodule for mspec"
  task :init do
    unless File.exist? "mspec/bin/mspec"
      puts "Initializing mspec submodule..."
      rm_rf "mspec"
      sh "git submodule init mspec"
      sh "git submodule update mspec"
    end
  end

  desc "Synchronize mspec submodule to current remote version"
  task :sync do
    Dir.chdir "mspec" do
      sh "git fetch"
      sh "git rebase origin"
    end
    version = `git log --pretty=oneline -1 mspec`[0..7]
    sh "git add mspec"
    sh "git commit -m 'Updated MSpec submodule to #{version}'"
  end

  desc "Update mspec sources to current submodule version"
  task :update do
    sh "git submodule update mspec"
  end

  task :pull => :update
end

# MISC TASKS

desc "Build task for CruiseControl"
task :ccrb => [:build, 'spec:ci']

namespace :test do
  desc "Run CI client daemon in incremental mode."
  task :daemon do
    sh "tools/cluster_test/ci.rb -i"
  end

  desc "Run CI client daemon in full build mode."
  task :daemon_full do
    sh "tools/cluster_test/ci.rb"
  end
end
