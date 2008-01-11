require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "UDPSocket.open" do
  after(:each) do
    @socket.close if @socket && !@socket.closed?
    @server.close if @server && !@server.closed?
  end

  it "returns a socket that can be written to and read from" do
    @ready = false
    server_thread = Thread.new do
      @server = UDPSocket.open
      begin
        @server.bind(nil,SocketSpecs.port)
      rescue Object => e
        puts "unable to bind."
        raise e
      end
      @ready = true
      msg1 = @server.recvfrom(64)
      msg1[0].should == "ad hoc"
      msg1[1][0].should == "AF_INET"
      msg1[1][1].should be_kind_of(Fixnum)
      msg1[1][3].should == "127.0.0.1"

      msg2 = @server.recvfrom(64)
      msg2[0].should == "connection-based"
      msg2[1][0].should == "AF_INET"
      msg2[1][1].should be_kind_of(Fixnum)
      msg2[1][3].should == "127.0.0.1"
    end

    Thread.pass while server_thread.status and !@ready

    UDPSocket.open.send("ad hoc", 0, 'localhost',SocketSpecs.port)

    @socket = UDPSocket.open
    @socket.connect('localhost',SocketSpecs.port)
    @socket.send("connection-based", 0)

    server_thread.join
  end
end
