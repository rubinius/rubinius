require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "TCPServer#accept" do
  before(:each) do
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
    @thread = Thread.new do
      client = @server.accept
      @data = client.read(5)
      client << "goodbye"
      client.close
    end
  end

  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end

  it "accepts what is written by the client" do
    @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    @socket.write('hello')
    @socket.read(7).should == 'goodbye'
    @thread.join
    @data.should == 'hello'
  end
end
