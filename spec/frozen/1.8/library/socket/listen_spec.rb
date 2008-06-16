require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

include Socket::Constants

describe "Socket#listen" do
  it "verifies we can listen for incoming connections" do
    begin
      sock = Socket.new(AF_INET, SOCK_STREAM, 0);
      # Port number taken from random.org, has no meaning
      sockaddr = Socket.pack_sockaddr_in(23986, "127.0.0.1");
      sock.setsockopt(Socket::SOL_SOCKET, Socket::SO_REUSEADDR, true);
      sock.bind(sockaddr);
      sock.listen(1);
    ensure
      sock.close();
    end
  end
end
