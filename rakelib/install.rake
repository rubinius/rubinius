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
    destdir = ENV['DESTDIR'] || ''
    dir = File.expand_path(File.join(destdir, BUILD_CONFIG[:prefixdir], name))

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

  options, name = name, nil if name.kind_of? Hash
  name = source[prefix.size..-1] unless name

  dest_name = File.join(dest, name)
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

def install_extra_bins(prefix, target)
  install_file "#{prefix}/testrb", prefix, "#{target}#{BUILD_CONFIG[:bindir]}", :mode => 0755
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

def install_lib_excludes(prefix, list)
  list.exclude("#{prefix}/**/ext/melbourne/build/*.*")
  BUILD_CONFIG[:supported_versions].each do |ver|
    unless BUILD_CONFIG[:language_version] == ver
      list.exclude(%r[^#{prefix}/#{ver}/.*])
    end
  end
end

def install_build_lib(prefix, target)
  list = FileList["#{prefix}/**/*.*", "#{prefix}/**/*"]
  install_lib_excludes prefix, list

  list.each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:libdir]}"
  end
end

def install_lib(prefix, target)
  list = FileList["#{prefix}/**/*.rb", "#{prefix}/**/rubygems/**/*"]
  install_lib_excludes prefix, list

  re = %r[/lib/#{BUILD_CONFIG[:language_version]}/]
  list.each do |source|
    name = source.gsub(re, "/lib/")[prefix.size..-1]
    install_file source, prefix, "#{target}#{BUILD_CONFIG[:libdir]}", name
  end
end

def install_transcoders(prefix, target)
  FileList["#{prefix}/*#{$dlext}"].each do |source|
    install_file source, prefix, "#{target}#{BUILD_CONFIG[:encdir]}", :mode => 0755
  end
end

def install_tooling(prefix, target)
  FileList["#{prefix}/tooling/**/*.#{$dlext}"].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:libdir]}"
  end
end

def install_cext(prefix, target)
  list = FileList["#{prefix}/**/ext/**/*.#{$dlext}"]
  list.exclude("**/melbourne/build/*.*")
  install_lib_excludes prefix, list

  re = %r[/lib/#{BUILD_CONFIG[:language_version]}/]
  list.each do |source|
    name = source.gsub(re, "/lib/")[prefix.size..-1]
    install_file source, prefix, "#{target}#{BUILD_CONFIG[:libdir]}", name
  end
end

def install_documentation(prefix, target)
  FileList["#{prefix}/rubinius/documentation/**/*"].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:libdir]}"
  end
end

def install_manpages(prefix, target)
  FileList["#{prefix}/**/*"].each do |name|
    install_file name, prefix, "#{target}#{BUILD_CONFIG[:mandir]}"
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

  task :extra_bins do
    if BUILD_CONFIG[:stagingdir]
      install_extra_bins "#{BUILD_CONFIG[:sourcedir]}/bin", BUILD_CONFIG[:stagingdir]
    end
  end

  task :capi_include do
    if BUILD_CONFIG[:stagingdir]
      v = BUILD_CONFIG[:language_version]
      install_capi_include "#{BUILD_CONFIG[:sourcedir]}/vm/capi/#{v}/include",
                           "#{BUILD_CONFIG[:stagingdir]}#{BUILD_CONFIG[:"includedir"]}"
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

  task :manpages do
    if BUILD_CONFIG[:stagingdir]
      install_manpages "#{BUILD_CONFIG[:sourcedir]}/doc/generated/vm/man", BUILD_CONFIG[:stagingdir]
    end
  end
end

namespace :install do
  desc "Install all the Rubinius files. Use DESTDIR environment variable " \
       "to specify custom installation location."
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
include: #{prefix}#{BUILD_CONFIG[:includedir]}

Please ensure that the paths to these directories are writable
by the current user. Otherwise, run 'rake install' with the
oppropriate command to elevate permissions (eg su, sudo).
        EOM

        exit(1)
      else
        stagingdir = BUILD_CONFIG[:stagingdir]
        destdir = ENV['DESTDIR'] || ''
        prefixdir = File.join(destdir, BUILD_CONFIG[:prefixdir])

        install_capi_include "#{stagingdir}#{BUILD_CONFIG[:includedir]}",
                             "#{prefixdir}#{BUILD_CONFIG[:includedir]}"

        install_runtime "#{stagingdir}#{BUILD_CONFIG[:runtimedir]}", prefixdir

        install_kernel "#{stagingdir}#{BUILD_CONFIG[:kerneldir]}", prefixdir

        install_lib "#{stagingdir}#{BUILD_CONFIG[:libdir]}", prefixdir

        install_transcoders "#{stagingdir}#{BUILD_CONFIG[:encdir]}", prefixdir

        install_tooling "#{stagingdir}#{BUILD_CONFIG[:libdir]}", prefixdir

        install_documentation "#{stagingdir}#{BUILD_CONFIG[:libdir]}", prefixdir

        install_manpages "#{stagingdir}#{BUILD_CONFIG[:mandir]}", prefixdir

        install_cext "#{stagingdir}#{BUILD_CONFIG[:libdir]}", prefixdir

        bin = "#{BUILD_CONFIG[:bindir]}/#{BUILD_CONFIG[:program_name]}"
        install_bin "#{stagingdir}#{bin}", prefixdir

        install_extra_bins "#{stagingdir}/#{BUILD_CONFIG[:bindir]}", prefixdir

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

