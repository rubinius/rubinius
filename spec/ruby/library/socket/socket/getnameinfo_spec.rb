require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

require 'socket'

describe "Socket#getnameinfo" do
  before :all do
    @reverse_lookup = BasicSocket.do_not_reverse_lookup
    BasicSocket.do_not_reverse_lookup = true
  end

  after :all do
    BasicSocket.do_not_reverse_lookup = @reverse_lookup
  end

  it "gets the name information and don't resolve it" do
    sockaddr = Socket.sockaddr_in SocketSpecs.port, '127.0.0.1'
    name_info = Socket.getnameinfo(sockaddr, Socket::NI_NUMERICHOST | Socket::NI_NUMERICSERV)
    name_info.should == ['127.0.0.1', "#{SocketSpecs.port}"]
  end

  it "gets the name information and resolve the host" do
    sockaddr = Socket.sockaddr_in SocketSpecs.port, '127.0.0.1'
    name_info = Socket.getnameinfo(sockaddr, Socket::NI_NUMERICSERV)
    name_info.should == ['localhost', "#{SocketSpecs.port}"]
  end

  it "gets the name information and resolves the service" do
    sockaddr = Socket.sockaddr_in 80, '127.0.0.1'
    name_info = Socket.getnameinfo(sockaddr)
    name_info.size.should == 2
    name_info[0].should == "localhost"
    # see http://www.iana.org/assignments/port-numbers
    name_info[1].should =~ /^(www|http|www-http)$/
  end
end
