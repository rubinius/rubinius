require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "TCPServer#accept" do
  before(:each) do
    @data = []
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
    @read = false
    @thread = Thread.new do
      @listening = true
      client = @server.accept
      @data << client.read(5)
      client << "hello"
      @read = true
      client.close
    end
    
    while @thread.status and !@listening
      sleep 0.2
      Thread.pass
    end
    sleep 0.2
  end

  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end

  it "accepts what is written by the client" do
    @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    @socket.write('hello')
    out = @socket.read(5)
    @thread.join
    @data.should == ['hello']
  end
end
