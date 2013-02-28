require 'benchmark'
require 'benchmark/ips'

require 'rubygems'
require 'rubygems/commands/install_command'

ERUBIS_VERSION = '2.7.0'

begin
  gem 'erubis', ERUBIS_VERSION
  require 'erubis'
rescue LoadError 
  inst = Gem::DependencyInstaller.new 
  inst.install 'erubis', ERUBIS_VERSION
  gem 'erubis', ERUBIS_VERSION
  require 'erubis'
end

erb_template = File.read(File.join(File.dirname(__FILE__), 'template.erb'))
eruby = Erubis::Eruby.new(erb_template)
feruby = Erubis::FastEruby.new(erb_template)

Benchmark.ips do |x|
  x.report "Eruby result" do
    eruby.result
  end
  x.report "FastEruby result" do
    feruby.result
  end
end


