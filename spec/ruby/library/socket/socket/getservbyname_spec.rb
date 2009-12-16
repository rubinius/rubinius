require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Socket#getservbyname" do

  it "identifies service ports " do
    Socket.getservbyname('http').should == 80
    Socket.getservbyname('http', 'tcp').should == 80
    Socket.getservbyname('domain', 'udp').should == 53
    Socket.getservbyname('daytime').should == 13
  end

  it "raises a SocketError when the service or port is invalid" do
    lambda { Socket.getservbyname('invalid') }.should raise_error(SocketError)
  end
end
