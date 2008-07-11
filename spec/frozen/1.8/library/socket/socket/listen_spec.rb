require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

include Socket::Constants

describe "Socket#listen" do
  after :each do
    @socket.close if @socket && !@socket.closed?
  end

  it "verifies we can listen for incoming connections" do
    @socket = Socket.new(AF_INET, SOCK_STREAM, 0)
    # Port number taken from random.org, has no meaning
    sockaddr = Socket.pack_sockaddr_in(23986, "127.0.0.1")
    @socket.setsockopt(Socket::SOL_SOCKET, Socket::SO_REUSEADDR, true)
    @socket.bind(sockaddr)
    @socket.listen(1).should == 0
  end
end
