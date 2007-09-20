require File.dirname(__FILE__) + '/../spec_helper'
require 'socket'
@port = 40000

=begin
describe "Socket" do
  it "should inherit from BasicSocket and IO" do
    Socket.superclass.should == BasicSocket
    BasicSocket.superclass.should == IO
  end
end

describe "Socket's class hierarchy" do
  it "should have an IPSocket in parallel to Socket" do
    Socket.ancestors.include?(IPSocket).should == false
    IPSocket.ancestors.include?(Socket).should == false
    IPSocket.superclass.should == BasicSocket
    TCPSocket.superclass.should == IPSocket
    UDPSocket.superclass.should == IPSocket
  end

  it "should have a UNIXSocket in parallel to Socket" do
    Socket.ancestors.include?(UNIXSocket).should == false
    UNIXSocket.ancestors.include?(Socket).should == false
    UNIXSocket.superclass.should == BasicSocket
  end
end

describe "Server class hierarchy" do
  it "should contain UNIXServer" do
    UNIXServer.superclass.should == UNIXSocket
  end
end
=end

describe "TCPServer.new" do
  after(:each) do
    @server.close if @server
  end
  it "should bind to a host and a port" do
    @server = TCPServer.new('127.0.0.1', @port)
  end
end

=begin
describe "TCPServer#accept" do
  before(:each) do
    @server = TCPServer.new('127.0.0.1', @port)
    @server.listen 1
  end
  after(:each) do
    @server.close
  end
end

describe "TCPServer#accept_nonblock" do
end

describe "TCPServer#listen" do
  before(:each) do
    @server = TCPServer.new('127.0.0.1', @port)
  end
  after(:each) do
    @server.close
  end
  it "should always return zero" do
    @server.listen(1).should == 0
  end
end
=end

=begin
describe "TCPServer receiving data" do
  before(:each) do
    @data = []
    @read = false
    @thread = Thread.new do
      @server = TCPServer.new('127.0.0.1', @port)
      while (client = @server.accept) do
        @data << client.read(5)
        @read = true
        client.close
      end
    end
  end
  after(:each) do
    @server.close
  end

  it "should accept what is written by the client" do
    @client = TCPSocket.new('127.0.0.1', @port)
    @client.write('hello')
    nil until @read
    @data.should == ['hello']
  end
end
=end

describe "TCPServer#sysaccept" do
end
