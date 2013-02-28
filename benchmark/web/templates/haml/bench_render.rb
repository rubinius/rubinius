require 'benchmark'
require 'benchmark/ips'

require 'rubygems'
require 'rubygems/commands/install_command'

HAML_VERSION = '4.0.0'

begin
  gem 'haml', HAML_VERSION
  require 'haml'
rescue LoadError 
  inst = Gem::DependencyInstaller.new 
  inst.install 'haml', HAML_VERSION
  gem 'haml', HAML_VERSION
  require 'haml'
end

engine = Haml::Engine.new(File.read(File.join(File.dirname(__FILE__), 'template.haml')))

Benchmark.ips do |x|
  x.report "Haml render" do
    engine.render
  end
  
end

