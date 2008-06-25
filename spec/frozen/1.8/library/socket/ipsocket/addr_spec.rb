require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Socket::IPSocket#addr" do

  before :each do
    @socket = TCPServer.new("127.0.0.1", SocketSpecs.port)
  end

  after :each do
    @socket.close unless @socket.closed?
  end

  it "returns an array with the socket's information" do
    BasicSocket.do_not_reverse_lookup = false
    addrinfo = @socket.addr
    addrinfo[0].should == "AF_INET"
    addrinfo[1].should == SocketSpecs.port
    addrinfo[2].should == "localhost"
    addrinfo[3].should == "127.0.0.1"
  end

  it "returns an address in the array if do_not_reverse_lookup is true" do
    BasicSocket.do_not_reverse_lookup = true
    addrinfo = @socket.addr
    addrinfo[0].should == "AF_INET"
    addrinfo[1].should == SocketSpecs.port
    addrinfo[2].should == "127.0.0.1"
    addrinfo[3].should == "127.0.0.1"
  end
end
