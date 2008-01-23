require File.dirname(__FILE__) + '/../../spec_helper'
require 'drb'

describe "DRb.start_server" do    
  it "should run a basic remote call" do
    class TestServer
      def add(*args)
        args.inject {|n,v| n+v}
      end
    end
    
    DRb.start_service('druby://localhost:9001', TestServer.new)
    
    obj = DRbObject.new(nil, 'druby://localhost:9001')
    obj.add(1,2,3).should == 6
    DRb.stop_service
  end

  it "should run a basic remote call passing a block" do
    class TestServer
      def add1(x)        
        return (yield x)+1
      end
    end
    
    DRb.start_service('druby://localhost:9001', TestServer.new)
    
    obj = DRbObject.new(nil, 'druby://localhost:9001')
    obj.add1(2) do |i|
      i.should == 2
      i+1
    end.should == 4
    DRb.stop_service
  end
end

