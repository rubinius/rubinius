require 'benchmark'
require 'benchmark/ips'

class HelloWorld
  def call(env)
    [200, {"Content-Type" => "text/html"}, "Hello world"]
  end
end

class RackHandler

  def initialize(app)
    @app = app
  end
  
  def process
    @app.call({})
  end

end

Benchmark.ips do |x|

  handler = RackHandler.new(HelloWorld.new)

  x.report 'Rack hello world' do
    handler.process
  end

end

