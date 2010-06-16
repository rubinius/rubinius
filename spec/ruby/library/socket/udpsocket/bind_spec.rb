require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

describe "UDPSocket.bind" do

  before :each do
    @socket = UDPSocket.new
  end

  after :each do
    @socket.close unless @socket.closed?
  end

  it "binds the socket to a port" do
    @socket.bind(SocketSpecs.hostname, SocketSpecs.port)

    lambda { @socket.bind(SocketSpecs.hostname, SocketSpecs.port) }.should raise_error
  end

  # This is quarantined because it fails randomly on linux.
  # I believe it's because of ipv4/ipv6 interaction
  quarantine! do
    it "receives a hostname and a port" do
      @socket.bind(SocketSpecs.hostname, SocketSpecs.port)

      port, host = Socket.unpack_sockaddr_in(@socket.getsockname)

      host.should == "127.0.0.1"
      port.should == SocketSpecs.port
    end
  end

  it "binds to INADDR_ANY if the hostname is empty" do
    @socket.bind("", SocketSpecs.port)
    port, host = Socket.unpack_sockaddr_in(@socket.getsockname)
    host.should == "0.0.0.0"
  end
end
