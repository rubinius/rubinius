require File.expand_path('../../../../spec_helper', __FILE__)
require 'socket'

describe "Addrinfo.tcp" do

  before :each do
    @addrinfo = Addrinfo.udp("localhost", "smtp")
  end

  it "creates a addrinfo for a tcp socket" do
    ["::1", "127.0.0.1"].should include(@addrinfo.ip_address)
    [Socket::PF_INET, Socket::PF_INET6].should include(@addrinfo.pfamily)
    @addrinfo.ip_port.should == 25
    @addrinfo.socktype.should == Socket::SOCK_DGRAM
    @addrinfo.protocol.should == Socket::IPPROTO_UDP
  end

end
