require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

include Socket::Constants

describe "Socket#bind" do

  before :each do
    @sock = Socket.new(AF_INET, SOCK_DGRAM, 0);    
    @sockaddr = Socket.pack_sockaddr_in(SocketSpecs.port, "127.0.0.1");
  end

  after :each do
    @sock.closed?.should be_false
    @sock.close
  end                           

  it "binds to a port" do
    lambda { @sock.bind(@sockaddr) }.should_not raise_error
  end
  
  it "raises an error if we try to bind to an already bound port" do
    @sock.bind(@sockaddr);

    sock2 = Socket.new(AF_INET, SOCK_DGRAM, 0);
    lambda { sock2.bind(@sockaddr) }.should raise_error;

    sock2.close unless sock2.closed?
  end
end
