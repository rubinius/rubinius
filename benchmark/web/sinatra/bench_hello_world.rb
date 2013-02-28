require 'benchmark'
require 'benchmark/ips'

require 'rubygems'
require 'rubygems/commands/install_command'

SINATRA_VERSION = '1.3.5'

begin
  gem 'sinatra', SINATRA_VERSION
  require 'sinatra'
rescue LoadError 
  inst = Gem::DependencyInstaller.new
  inst.install 'sinatra', SINATRA_VERSION
  gem 'sinatra', SINATRA_VERSION
  require 'sinatra'
end

module Sinatra
  class Application < Base
    def self.run?
      false
    end
  end
end

class MyApp < Sinatra::Base
  
  get '/hello' do
    "Hello world"
  end
end

Benchmark.ips do |x|
  x.report "Sinatra hello world" do
    MyApp.call({"PATH_INFO" => "/hello", "REQUEST_METHOD" => "GET", "rack.input" => ""})
  end
  
end

