require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "BasicSocket#setsockopt" do
  
  before(:each) do
    @sock = Socket.new(Socket::AF_INET, Socket::SOCK_STREAM, 0)
  end

  after :each do
    @sock.close unless @sock.closed?
  end
  
  it "sets the socket linger to 0" do
    linger = [0, 0].pack("ii")
    @sock.setsockopt(Socket::SOL_SOCKET, Socket::SO_LINGER, linger).should == 0
  end
end
