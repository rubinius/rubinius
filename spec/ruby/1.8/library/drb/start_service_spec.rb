require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/test_server'
require 'drb'

describe "DRb.start_service" do
  it "should run a basic remote call" do    
    lambda { DRb.current_server }.should raise_error(DRb::DRbServerNotFound)
    server = DRb.start_service('druby://localhost:9001', TestServer.new)
    DRb.current_server.should == server
    obj = DRbObject.new(nil, 'druby://localhost:9001')
    obj.add(1,2,3).should == 6
    DRb.stop_service
    lambda { DRb.current_server }.should raise_error(DRb::DRbServerNotFound)
  end

  it "should run a basic remote call passing a block" do
    lambda { DRb.current_server }.should raise_error(DRb::DRbServerNotFound)
    server = DRb.start_service('druby://localhost:9001', TestServer.new)
    DRb.current_server.should == server
    obj = DRbObject.new(nil, 'druby://localhost:9001')
    obj.add_yield(2) do |i|
      i.should == 2
      i+1
    end.should == 4
    DRb.stop_service
    lambda { DRb.current_server }.should raise_error(DRb::DRbServerNotFound)
  end
end
