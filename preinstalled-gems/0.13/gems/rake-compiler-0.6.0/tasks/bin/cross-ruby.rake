#--
# Cross-compile ruby, using Rake
#
# This source code is released under the MIT License.
# See LICENSE file for details
#++

#
# This code is inspired and based on notes from the following sites:
#
# http://tenderlovemaking.com/2008/11/21/cross-compiling-ruby-gems-for-win32/
# http://github.com/jbarnette/johnson/tree/master/cross-compile.txt
# http://eigenclass.org/hiki/cross+compiling+rcovrt
#
# This recipe only cleanup the dependency chain and automate it.
# Also opens the door to usage different ruby versions 
# for cross-compilation.
#

require 'rake'
require 'rake/clean'
require 'yaml'

# load compiler helpers
# add lib directory to the search path
libdir = File.expand_path(File.join(File.dirname(__FILE__), '..', '..', 'lib'))
$LOAD_PATH.unshift(libdir) unless $LOAD_PATH.include?(libdir)

require 'rake/extensioncompiler'

USER_HOME = File.expand_path("~/.rake-compiler")
RUBY_CC_VERSION = "ruby-#{ENV['VERSION'] || '1.8.6-p287'}"
RUBY_SOURCE = ENV['SOURCE']

# grab the major "1.8" or "1.9" part of the version number
MAJOR = RUBY_CC_VERSION.match(/.*-(\d.\d).\d/)[1]

# Use Rake::ExtensionCompiler helpers to find the proper host
MINGW_HOST = Rake::ExtensionCompiler.mingw_host

# define a location where sources will be stored
directory "#{USER_HOME}/sources/#{RUBY_CC_VERSION}"
directory "#{USER_HOME}/builds/#{RUBY_CC_VERSION}"

# clean intermediate files and folders
CLEAN.include("#{USER_HOME}/sources/#{RUBY_CC_VERSION}")
CLEAN.include("#{USER_HOME}/builds/#{RUBY_CC_VERSION}")

# remove the final products and sources
CLOBBER.include("#{USER_HOME}/sources")
CLOBBER.include("#{USER_HOME}/builds")
CLOBBER.include("#{USER_HOME}/ruby/#{RUBY_CC_VERSION}")
CLOBBER.include("#{USER_HOME}/config.yml")

# ruby source file should be stored there
file "#{USER_HOME}/sources/#{RUBY_CC_VERSION}.tar.gz" => ["#{USER_HOME}/sources"] do |t|
  # download the source file using wget or curl
  chdir File.dirname(t.name) do
    if RUBY_SOURCE
      url = RUBY_SOURCE
    else
      url = "http://ftp.ruby-lang.org/pub/ruby/#{MAJOR}/#{File.basename(t.name)}"
    end
    sh "wget #{url} || curl -O #{url}"
  end
end

# Extract the sources
source_file = RUBY_SOURCE ? RUBY_SOURCE.split('/').last : "#{RUBY_CC_VERSION}.tar.gz"
file "#{USER_HOME}/sources/#{RUBY_CC_VERSION}" => ["#{USER_HOME}/sources/#{source_file}"] do |t|
  chdir File.dirname(t.name) do
    t.prerequisites.each { |f| sh "tar xfz #{File.basename(f)}" }
  end
end

# backup makefile.in
file "#{USER_HOME}/sources/#{RUBY_CC_VERSION}/Makefile.in.bak" => ["#{USER_HOME}/sources/#{RUBY_CC_VERSION}"] do |t|
  cp "#{USER_HOME}/sources/#{RUBY_CC_VERSION}/Makefile.in", t.name
end

# correct the makefiles
file "#{USER_HOME}/sources/#{RUBY_CC_VERSION}/Makefile.in" => ["#{USER_HOME}/sources/#{RUBY_CC_VERSION}/Makefile.in.bak"] do |t|
  content = File.open(t.name, 'rb') { |f| f.read }

  out = ""

  content.each_line do |line|
    if line =~ /^\s*ALT_SEPARATOR =/
      out << "\t\t    ALT_SEPARATOR = \"\\\\\\\\\"; \\\n"
    else
      out << line
    end
  end

  when_writing("Patching Makefile.in") {
    File.open(t.name, 'wb') { |f| f.write(out) }
  }
end

task :mingw32 do
  unless MINGW_HOST then
    warn "You need to install mingw32 cross compile functionality to be able to continue."
    warn "Please refer to your distribution/package manager documentation about installation."
    fail
  end
end

task :environment do
  ENV['ac_cv_func_getpgrp_void'] =  'no'
  ENV['ac_cv_func_setpgrp_void'] = 'yes'
  ENV['rb_cv_negative_time_t'] = 'no'
  ENV['ac_cv_func_memcmp_working'] = 'yes'
  ENV['rb_cv_binary_elf' ] = 'no'
end

# generate the makefile in a clean build location
file "#{USER_HOME}/builds/#{RUBY_CC_VERSION}/Makefile" => ["#{USER_HOME}/builds/#{RUBY_CC_VERSION}",
                                  "#{USER_HOME}/sources/#{RUBY_CC_VERSION}/Makefile.in"] do |t|

  options = [
    '--target=i386-mingw32',
    "--host=#{MINGW_HOST}",
    '--build=i686-linux',
    '--enable-shared',
    '--disable-install-doc',
    '--without-tk',
    '--without-tcl'
  ]

  chdir File.dirname(t.name) do
    prefix = File.expand_path("../../ruby/#{RUBY_CC_VERSION}")
    options << "--prefix=#{prefix}"
    sh File.expand_path("../../sources/#{RUBY_CC_VERSION}/configure"), *options
  end
end

# make
file "#{USER_HOME}/builds/#{RUBY_CC_VERSION}/ruby.exe" => ["#{USER_HOME}/builds/#{RUBY_CC_VERSION}/Makefile"] do |t|
  chdir File.dirname(t.prerequisites.first) do
    sh "make"
  end
end

# make install
file "#{USER_HOME}/ruby/#{RUBY_CC_VERSION}/bin/ruby.exe" => ["#{USER_HOME}/builds/#{RUBY_CC_VERSION}/ruby.exe"] do |t|
  chdir File.dirname(t.prerequisites.first) do
    sh "make install"
  end
end
task :install => ["#{USER_HOME}/ruby/#{RUBY_CC_VERSION}/bin/ruby.exe"]

desc "Update rake-compiler list of installed Ruby versions"
task 'update-config' do
  config_file = "#{USER_HOME}/config.yml"
  if File.exist?(config_file) then
    puts "Updating #{config_file}"
    config = YAML.load_file(config_file)
  else
    puts "Generating #{config_file}"
    config = {}
  end

  files = Dir.glob("#{USER_HOME}/ruby/**/rbconfig.rb").sort

  files.each do |rbconfig|
    version = rbconfig.match(/.*-(\d.\d.\d)/)[1]
    config["rbconfig-#{version}"] = rbconfig
    puts "Found Ruby version #{version} (#{rbconfig})"
  end

  when_writing("Saving changes into #{config_file}") {
    File.open(config_file, 'w') do |f|
      f.puts config.to_yaml
    end
  }
end

task :default do
  # Force the display of the available tasks when no option is given
  Rake.application.options.show_task_pattern = //
  Rake.application.display_tasks_and_comments
end

desc "Build #{RUBY_CC_VERSION} suitable for cross-platform development."
task 'cross-ruby' => [:mingw32, :environment, :install, 'update-config']
