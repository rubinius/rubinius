require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Socket#unpack_sockaddr_in" do

  it "decodes the port number of a packed sockaddr_in" do
    Socket.unpack_sockaddr_in( Socket.sockaddr_in(80, "") )[0].should == 80
    Socket.unpack_sockaddr_in( Socket.sockaddr_in(0, "") )[0].should == 0
    Socket.unpack_sockaddr_in( Socket.sockaddr_in(5000, "") )[0].should == 5000
  end

  it "decodes the host name of a packed sockaddr_in" do
    Socket.unpack_sockaddr_in( Socket.sockaddr_in(0, "127.0.0.1") )[1].should == "127.0.0.1"
    Socket.unpack_sockaddr_in( Socket.sockaddr_in(0, "500") )[1].should == "0.0.1.244"
  end

  not_compliant_on :rubinius do
    it "decodes an empty hostname of a packed sockaddr_in to 0.0.0.0" do
      Socket.unpack_sockaddr_in( Socket.sockaddr_in(0, "") )[1].should == "0.0.0.0"
    end
  end

end