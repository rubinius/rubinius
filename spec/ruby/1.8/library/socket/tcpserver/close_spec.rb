require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "TCPServer#close" do
  it "returns nil" do
    server = TCPServer.new('127.0.0.1', SocketSpecs.port)
    lambda { server.close.should == nil }.should_not raise_error
  end

  it "should raise an error if closed twice" do
    server = TCPServer.new('127.0.0.1', SocketSpecs.port)
    server.close
    lambda { server.close }.should raise_error(IOError)
  end
end
