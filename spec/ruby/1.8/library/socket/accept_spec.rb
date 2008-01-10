require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "TCPServer#accept" do
  before(:each) do
    @data = []
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
    @read = false
    @thread = Thread.new do
      begin
        client = @server.accept_nonblock
      rescue Errno::EAGAIN, Errno::ECONNABORTED, Errno::EPROTO, Errno::EINTR
        @listening = true
        IO.select([@server])
        retry
      end
      @data << client.read(5)
      @read = true
      client.close
    end
    Thread.pass until @listening
  end

  after(:each) do
    @server.close if @server
    @socket.close if @socket
  end

  it "accepts what is written by the client" do
    @socket = TCPSocket.new('127.0.0.1', SocketSpecs.port)
    @socket.write('hello')
    Thread.pass until @read
    @thread.join
    @data.should == ['hello']
  end
end
