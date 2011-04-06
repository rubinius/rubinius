# Tasks for installing Rubinius. There are two guidelines here:
#
#  1. Only use sudo if necessary
#  2. Build all Ruby files using the bootstrap Ruby implementation
#     and install the files with the 'install' command
#

def install_dir(lib)
  if fr = ENV['FAKEROOT']
    return File.join(fr, lib)
  end

  lib
end

install_dirs = [
  BUILD_CONFIG[:bindir],
  BUILD_CONFIG[:libdir],
  BUILD_CONFIG[:includedir],
  BUILD_CONFIG[:mandir],
  BUILD_CONFIG[:gemsdir]
]

# What the hell does this code do? We want to avoid sudo whenever
# possible. This code is based on the assumption that if A is a
# directory name, if any of the paths leading up to the full A are
# writable, then A can be created. So A is decomposed one directory
# at a time from the right-hand side. That path is checked for
# whether it is a directory. If it is and it is writable, we can
# create A. Otherwise, we can't create A and sudo is required.
def need_sudo?(dirs)
  dirs.each do |name|
    dir = install_dir(File.expand_path(name))

    until dir == "/"
      if File.directory? dir
        return true unless File.writable? dir
        break
      end

      dir = File.dirname dir
    end
  end

  return false
end

def need_install?
  File.expand_path(Dir.pwd) != install_dir(BUILD_CONFIG[:libdir])
end

def precompile(dir)
  (Dir["#{dir}/*.rb"] + Dir["#{dir}/**/*.rb"]).each do |file|
    Rubinius::Compiler.compile file, "#{file}c", 1, [:default]
  end
end

def install_file(source, pattern, dest)
  return if File.directory? source

  dest_name = install_dir File.join(dest, source.sub(pattern, ""))
  dir = File.dirname(dest_name)
  mkdir_p dir unless File.directory? dir

  install source, dest_name, :mode => 0644, :verbose => true
end

desc "Install Rubinius"
task :install => %w[ build install:build install:files ]

namespace :install do
  desc "Compile all lib Ruby files"
  task :build do
    if need_install?
      puts "Compiling library files for install..."
      precompile "lib"

      puts "Compiling pre-installed gem files for install..."
      precompile "preinstalled-gems/rubinius/0.13/gems"
    end
  end

  desc "Install all the Rubinius files"
  task :files do
    if need_sudo? install_dirs
      sh "sudo #{BUILD_CONFIG[:build_ruby]} -S #{BUILD_CONFIG[:build_rake]} install:files", :verbose => true
    elsif !need_install?
      puts "Install directory is the same as build directory, nothing to install"
    else
      install_dirs.each { |name| mkdir_p install_dir(name), :verbose => $verbose }

      FileList["vm/capi/include/*.h"].each do |name|
        install_file name, %r[^vm/capi/include], BUILD_CONFIG[:includedir]
      end

      FileList[
        'runtime/index',
        'runtime/signature',
        'runtime/platform.conf',
        'runtime/**/*.rb{a,c}',
        'runtime/**/load_order.txt'
      ].each do |name|
        install_file name, /^runtime/, BUILD_CONFIG[:runtime]
      end

      # Install the .rb files for the standard library. This is a
      # separate block from the .rbc files so that the .rb files
      # have an older timestamp and do not trigger recompiling.
      FileList['lib/**/*.rb'].each do |name|
        install_file name, /^lib/, BUILD_CONFIG[:lib_path]
      end

      FileList['lib/**/*.rbc'].each do |name|
        install_file name, /^lib/, BUILD_CONFIG[:lib_path]
      end

      # Install C++ extensions. Since we permit configuring where
      # to install the C extensions separately from the lib files,
      # we have to be more restrictive with the globbing.
      #
      # TODO: handle this better in daedalus.
      FileList["lib/tooling/**/*.#{$dlext}"].each do |name|
        install_file name, /^lib/, BUILD_CONFIG[:lib_path]
      end

      # Install the documentation site
      FileList['lib/rubinius/documentation/**/*'].each do |name|
        install_file name, /^lib/, BUILD_CONFIG[:lib_path]
      end

      # Install the C extensions for the standard library.
      FileList["lib/ext/**/*.#{$dlext}"].each do |name|
        install_file name, %r[^lib/ext], BUILD_CONFIG[:ext_path]
      end

      # Install pre-installed gems
      gems_dest = "#{BUILD_CONFIG[:gemsdir]}/rubinius/preinstalled"
      FileList["preinstalled-gems/data/**/*"].each do |name|
        install_file name, %r[^preinstalled-gems/data], gems_dest
      end

      FileList["preinstalled-gems/bin/*"].each do |name|
        install_file name, /^preinstalled-gems/, BUILD_CONFIG[:gemsdir]
      end

      # Install the Rubinius executable
      exe = "#{BUILD_CONFIG[:bindir]}/#{BUILD_CONFIG[:program_name]}"
      install "vm/vm", install_dir(exe), :mode => 0755, :verbose => true

      # Install the testrb command
      testrb = "#{BUILD_CONFIG[:bindir]}/testrb"
      install "bin/testrb", install_dir(testrb), :mode => 0755, :verbose => true

      # Create symlinks for common commands
      begin
        ["ruby", "rake", "gem", "irb", "rdoc", "ri"].each do |command|
          name = install_dir("#{BUILD_CONFIG[:bindir]}/#{command}")
          File.delete name if File.exists? name
          File.symlink BUILD_CONFIG[:program_name], name
        end
      rescue NotImplementedError
        # ignore
      end

      STDOUT.puts <<-EOM
--------

Successfully installed Rubinius #{BUILD_CONFIG[:version]}

Add '#{BUILD_CONFIG[:bindir]}' to your PATH. Available commands are:

  #{BUILD_CONFIG[:program_name]}, ruby, rake, gem, irb, rdoc, ri

  1. Run Ruby files with '#{BUILD_CONFIG[:program_name]} path/to/file.rb'
  2. Start IRB by running '#{BUILD_CONFIG[:program_name]}' with no arguments

      EOM
    end
  end
end

