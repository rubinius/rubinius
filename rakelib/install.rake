# Tasks for installing Rubinius. There are two guidelines here:
#
#  1. Only use sudo if necessary
#  2. Build all Ruby files using the bootstrap Ruby implementation
#     and install the files with the 'install' command
#

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
    dir = File.expand_path name

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
  File.expand_path(Dir.pwd) != BUILD_CONFIG[:libdir]
end

def precompile(dir)
  ruby "-Ilib lib/bin/compile.rb -V -T default #{dir}"
end

def install_file(source, pattern, dest)
  return if File.directory? source

  dest_name = File.join(dest, source.sub(pattern, ""))
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
      sh "sudo #{FileUtils::RUBY} -S rake install:files", :verbose => true
    elsif !need_install?
      puts "Install directory is the same as build directory, nothing to install"
    else
      install_dirs.each { |name| mkdir_p name, :verbose => $verbose }

      FileList["vm/capi/*.h"].each do |name|
        install_file name, %r[^vm/capi], BUILD_CONFIG[:includedir]
      end

      FileList[
        'runtime/index',
        'runtime/platform.conf',
        'runtime/**/*.rb{a,c}',
        'runtime/**/load_order.txt'
      ].each do |name|
        install_file name, /^runtime/, BUILD_CONFIG[:runtime]
      end

      # This is separate from below because it is an open question
      # whether we should install the .rb files along with the
      # .rbc files for the standard library. Some libraries,
      # like IRB, look for a specific .rb file and fail to work
      # if not finding it.
      FileList['lib/**/*.rb'].each do |name|
        install_file name, /^lib/, BUILD_CONFIG[:lib_path]
      end

      FileList['lib/**/*.rbc'].each do |name|
        install_file name, /^lib/, BUILD_CONFIG[:lib_path]
      end

      FileList["lib/ext/**/*.#{$dlext}"].each do |name|
        install_file name, %r[^lib/ext], BUILD_CONFIG[:ext_path]
      end

      # TODO: the preinstalled gems are a total mess right now, they
      # should not be in a source dir that includes any rbx version.
      # Furthermore, we are going from the source dir here because the
      # check on 'gems/rubinius' to copy them over will fail on any
      # repo that used gems before since `gem` created the 'gems/rubinius'
      # directory.
      gems_dest = "#{BUILD_CONFIG[:gemsdir]}/rubinius/#{BUILD_CONFIG[:libversion]}"
      FileList["preinstalled-gems/rubinius/0.13/**/*"].each do |name|
        install_file name, %r[^preinstalled-gems/rubinius/0.13], gems_dest
      end

      FileList["preinstalled-gems/bin/*"].each do |name|
        install_file name, /^preinstalled-gems/, BUILD_CONFIG[:gemsdir]
      end

      exe = "#{BUILD_CONFIG[:bindir]}/#{BUILD_CONFIG[:program_name]}"
      install "vm/vm", exe, :mode => 0755, :verbose => true
    end
  end
end

