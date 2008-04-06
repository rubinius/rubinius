require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "UDPSocket.send" do
  it "sends data in ad hoc mode" do
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

      @server.close
    end

    Thread.pass while server_thread.status and !@ready

    @socket = UDPSocket.open
    @socket.send("ad hoc", 0, 'localhost',SocketSpecs.port)
    @socket.close

    server_thread.join
  end

  it "sends data in connection mode" do
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
      msg1[0].should == "connection-based"
      msg1[1][0].should == "AF_INET"
      msg1[1][1].should be_kind_of(Fixnum)
      msg1[1][3].should == "127.0.0.1"

      @server.close
    end

    Thread.pass while server_thread.status and !@ready

    @socket = UDPSocket.open
    @socket.connect('localhost',SocketSpecs.port)
    @socket.send("connection-based", 0)
    @socket.close

    server_thread.join
  end
end
