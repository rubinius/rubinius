require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "TCPSocket#gethostbyname" do
  it "should resolve a hostname to an address" do
    a = TCPSocket.gethostbyname("localhost")
    if a[2] == Socket::AF_INET6
      a.should include("::1")
    else
      a.should include("127.0.0.1")
    end
  end
end
