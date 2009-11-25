module FileTemplateHelpers
  def template_rakefile
    <<-EOF
# add rake-compiler lib dir to the LOAD_PATH
$LOAD_PATH.unshift File.expand_path(File.join(File.dirname(__FILE__), '../..', 'lib'))

require 'rubygems'
require 'rake'

# load rakefile extensions (tasks)
Dir['tasks/*.rake'].each { |f| import f }
EOF
  end

  def template_rake_gemspec(gem_name)
    <<-EOF
require 'rake/gempackagetask'
SPEC = Gem::Specification.new do |s|
  s.name = "#{gem_name}"
  s.version = "0.1.0"
  s.summary = "#{gem_name} test gem for rake-compiler"

  s.files = FileList["ext/**/*.{rb,c,h}", "Rakefile", "tasks/*.rake", "lib/**/*.rb"]

  s.extensions = FileList["ext/**/extconf.rb"]

  s.has_rdoc = true

  s.homepage = 'http://github.com/luislavena/rake-compiler'
  s.rubyforge_project = 'TODO'

  s.authors = ["Luis Lavena"]
  s.email = ["luislavena@gmail.com"]
end

gem_package = Rake::GemPackageTask.new(SPEC) do |pkg|
  pkg.need_zip = false
  pkg.need_tar = false
end
EOF
  end

  def template_rake_extension(extension_name, gem_spec = nil)
    <<-EOF
require 'rake/extensiontask'
Rake::ExtensionTask.new("#{extension_name}"#{', SPEC' if gem_spec})
EOF
  end

  def template_rake_extension_with_platform(extension_name, platform)
    <<-EOF
require 'rake/extensiontask'
Rake::ExtensionTask.new("#{extension_name}", SPEC) do |ext|
  ext.platform = "#{platform}"
end
EOF
  end

  def template_rake_extension_cross_compile(extension_name, gem_spec = nil)
    <<-EOF
require 'rake/extensiontask'
Rake::ExtensionTask.new("#{extension_name}"#{', SPEC' if gem_spec}) do |ext|
  ext.cross_compile = true
end
EOF
  end

  def template_rake_extension_multi_cross_compile(extension_name)
    <<-EOF
require 'rake/extensiontask'
Rake::ExtensionTask.new("#{extension_name}", SPEC) do |ext|
  ext.cross_compile = true
  ext.cross_platform = ['i386-mswin32', 'i386-mingw32']
end
EOF
  end

  def template_extconf(extension_name)
    <<-EOF
require 'mkmf'
create_makefile("#{extension_name}")
EOF
  end

  def template_source_c(extension_name)
    <<-EOF
#include "source.h"
void Init_#{extension_name}()
{
  printf("source.c of extension #{extension_name}\\n");
}
EOF
  end

  def template_source_h
    <<-EOF
#include "ruby.h"
EOF
  end
end

World(FileTemplateHelpers)
