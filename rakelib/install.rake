# Tasks for installing Rubinius. There are two guidelines here:
#
#  1. Only use sudo if necessary
#  2. Build all Ruby files using the bootstrap Ruby implementation
#     and install the files with the 'install' command
#

desc "Install Rubinius"
task :install => %w[build:build gems:install install:files install:done]

# Determine if all the targets for the install directories are writable
# decomposing each candidate directory from the right side and checking if
# that path is writable. If not, we require explicit permission.
def need_permission?
  FileList["#{BUILD_CONFIG[:stagingdir]}/*"].each do |name|
    dir = File.expand_path "#{BUILD_CONFIG[:prefixdir]}/#{name}"

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

def install_file(source, prefix, dest, name=nil, options={})
  return if File.directory? source

  dest_name = File.join(dest, source[prefix.size..-1])
  dir = File.dirname dest_name
  mkdir_p dir, :verbose => $verbose unless File.directory? dir

  options[:mode] ||= 0644
  options[:verbose] ||= $verbose

  install source, dest_name, options
end

def install_bin(source, target)
  bin = "#{target}#{BUILD_CONFIG[:bindir]}/#{BUILD_CONFIG[:program_name]}"
  dir = File.dirname bin
  mkdir_p dir, :verbose => $verbose unless File.directory? dir

  install source, bin, :mode => 0755, :verbose => $verbose

  # Create symlinks for common commands
  if BUILD_CONFIG[:use_bin_links]
    begin
      BUILD_CONFIG[:bin_links].each do |name|
        link = "#{target}/#{BUILD_CONFIG[:bindir]}/#{name}"
        File.delete link if File.exists? link
        File.symlink BUILD_CONFIG[:program_name], link
      end
    rescue NotImplementedError
      # ignore
    end
  end
end

def install_runtime(prefix, target)
  FileList[
    "#{prefix}/platform.conf",
    "#{prefix}/**/index",
    "#{prefix}/**/signature",
    "#{prefix}/**/*.rb{a,c}",
    "#{prefix}/**/load_order*.txt"
  ].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:runtimedir]}"
  end
end

def install_kernel(prefix, target)
  FileList["#{prefix}/**/*.rb"].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:kerneldir]}"
  end
end

def install_capi_include(prefix, destination)
  FileList["#{prefix}/**/*.h", "#{prefix}/**/*.hpp"].each do |name|
    install_file name, prefix, destination
  end
end

def install_build_lib(prefix, target)
  FileList["#{prefix}/**/*.*", "#{prefix}/**/*"].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:libdir]}"
  end
end

def install_lib(prefix, target)
  FileList["#{prefix}/**/*.rb"].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:libdir]}"
  end
end

def install_tooling(prefix, target)
  FileList["#{prefix}/tooling/**/*.#{$dlext}"].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:libdir]}"
  end
end

def install_cext(prefix, target)
  FileList["#{prefix}/**/ext/**/*.#{$dlext}"].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:libdir]}"
  end
end

def install_documentation(prefix, target)
  FileList["#{prefix}/rubinius/documentation/**/*"].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:libdir]}"
  end
end

def install_gems(prefix, target)
  FileList["#{prefix}/**/*.*", "#{prefix}/**/*"].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:gemsdir]}"
  end
end

namespace :stage do
  task :bin do
    install_bin "#{BUILD_CONFIG[:sourcedir]}/vm/vm", BUILD_CONFIG[:sourcedir]

    if BUILD_CONFIG[:stagingdir]
      install_bin "#{BUILD_CONFIG[:sourcedir]}/vm/vm", BUILD_CONFIG[:stagingdir]

      name = BUILD_CONFIG[:program_name]
      mode = File::CREAT | File::TRUNC | File::WRONLY
      File.open("#{BUILD_CONFIG[:sourcedir]}/bin/#{name}", mode, 0755) do |f|
        f.puts <<-EOS
#!/bin/sh
#
# Rubinius has been configured to be installed. This convenience
# wrapper enables running Rubinius from the staging directories.

export RBX_PREFIX_PATH=#{BUILD_CONFIG[:stagingdir]}
EXE=$(basename $0)

exec #{BUILD_CONFIG[:stagingdir]}#{BUILD_CONFIG[:bindir]}/$EXE "$@"
        EOS
      end
    end
  end

  task :capi_include do
    if BUILD_CONFIG[:stagingdir]
      install_capi_include "#{BUILD_CONFIG[:sourcedir]}/vm/capi/18/include",
                           "#{BUILD_CONFIG[:stagingdir]}#{BUILD_CONFIG[:include18dir]}"
      install_capi_include "#{BUILD_CONFIG[:sourcedir]}/vm/capi/19/include",
                           "#{BUILD_CONFIG[:stagingdir]}#{BUILD_CONFIG[:include19dir]}"
      install_capi_include "#{BUILD_CONFIG[:sourcedir]}/vm/capi/20/include",
                           "#{BUILD_CONFIG[:stagingdir]}#{BUILD_CONFIG[:include20dir]}"
    end
  end

  task :lib do
    if BUILD_CONFIG[:stagingdir]
      install_build_lib "#{BUILD_CONFIG[:sourcedir]}/lib", BUILD_CONFIG[:stagingdir]
    end
  end

  task :tooling do
    if BUILD_CONFIG[:stagingdir]
      install_tooling "#{BUILD_CONFIG[:sourcedir]}/lib/tooling", BUILD_CONFIG[:stagingdir]
    end
  end

  task :runtime do
    if BUILD_CONFIG[:stagingdir]
      install_runtime "#{BUILD_CONFIG[:sourcedir]}/runtime", BUILD_CONFIG[:stagingdir]
    end
  end

  task :kernel do
    if BUILD_CONFIG[:stagingdir]
      install_kernel "#{BUILD_CONFIG[:sourcedir]}/kernel", BUILD_CONFIG[:stagingdir]
    end
  end

  task :documentation do
    if BUILD_CONFIG[:stagingdir]
      install_documentation "#{BUILD_CONFIG[:sourcedir]}/lib", BUILD_CONFIG[:stagingdir]
    end
  end
end

namespace :install do
  desc "Install all the Rubinius files"
  task :files do
    if BUILD_CONFIG[:stagingdir]
      if need_permission?
        prefix = BUILD_CONFIG[:prefixdir]
        STDERR.puts <<-EOM
Rubinius has been configured for the following paths:

bin:     #{prefix}#{BUILD_CONFIG[:bindir]}
lib:     #{prefix}#{BUILD_CONFIG[:libdir]}
runtime: #{prefix}#{BUILD_CONFIG[:runtimedir]}
kernel:  #{prefix}#{BUILD_CONFIG[:kerneldir]}
site:    #{prefix}#{BUILD_CONFIG[:sitedir]}
vendor:  #{prefix}#{BUILD_CONFIG[:vendordir]}
man:     #{prefix}#{BUILD_CONFIG[:mandir]}
gems:    #{prefix}#{BUILD_CONFIG[:gemsdir]}

Please ensure that the paths to these directories are writable
by the current user. Otherwise, run 'rake install' with the
oppropriate command to elevate permissions (eg su, sudo).
        EOM

        exit(1)
      else
        stagingdir = BUILD_CONFIG[:stagingdir]
        prefixdir = BUILD_CONFIG[:prefixdir]

        install_capi_include "#{stagingdir}#{BUILD_CONFIG[:include18dir]}",
                             "#{prefixdir}#{BUILD_CONFIG[:include18dir]}"
        install_capi_include "#{stagingdir}#{BUILD_CONFIG[:include19dir]}",
                             "#{prefixdir}#{BUILD_CONFIG[:include19dir]}"
        install_capi_include "#{stagingdir}#{BUILD_CONFIG[:include20dir]}",
                             "#{prefixdir}#{BUILD_CONFIG[:include20dir]}"

        install_runtime "#{stagingdir}#{BUILD_CONFIG[:runtimedir]}", prefixdir

        install_kernel "#{stagingdir}#{BUILD_CONFIG[:kerneldir]}", prefixdir

        install_lib "#{stagingdir}#{BUILD_CONFIG[:libdir]}", prefixdir

        install_tooling "#{stagingdir}#{BUILD_CONFIG[:libdir]}", prefixdir

        install_documentation "#{stagingdir}#{BUILD_CONFIG[:libdir]}", prefixdir

        install_cext "#{stagingdir}#{BUILD_CONFIG[:libdir]}", prefixdir

        bin = "#{BUILD_CONFIG[:bindir]}/#{BUILD_CONFIG[:program_name]}"
        install_bin "#{stagingdir}#{bin}", prefixdir

        install_gems "#{stagingdir}#{BUILD_CONFIG[:gemsdir]}", prefixdir

        # Install the testrb command
        testrb = "#{prefixdir}#{BUILD_CONFIG[:bindir]}/testrb"
        install "bin/testrb", testrb, :mode => 0755, :verbose => $verbose
      end
    end
  end

  task :done do
    STDOUT.puts <<-EOM
--------

Successfully installed Rubinius #{BUILD_CONFIG[:version]}

Add '#{BUILD_CONFIG[:prefixdir]}#{BUILD_CONFIG[:bindir]}' to your PATH. Available commands are:

  #{BUILD_CONFIG[:program_name]}, ruby, rake, gem, irb, rdoc, ri

  1. Run Ruby files with '#{BUILD_CONFIG[:program_name]} path/to/file.rb'
  2. Start IRB by running '#{BUILD_CONFIG[:program_name]}' with no arguments

    EOM
  end
end

