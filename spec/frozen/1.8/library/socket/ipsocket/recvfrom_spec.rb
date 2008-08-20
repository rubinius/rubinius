require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Socket::IPSocket#recvfrom" do

  before :each do
    @server = TCPServer.new("127.0.0.1", SocketSpecs.port)
    @client = TCPSocket.new("127.0.0.1", SocketSpecs.port)
  end

  after :each do
    @server.close unless @server.closed?
    @client.close unless @client.closed?
  end
  
  it "reads data from the connection" do
    data = nil
    t = Thread.new do
      client = @server.accept
      data = client.recvfrom(6)
      client.close
    end

    @client.send('hello', 0)
    t.join

    data.first.should == 'hello'
  end

  it "reads up to len bytes" do
    data = nil
    t = Thread.new do
      client = @server.accept
      data = client.recvfrom(3)
      client.close
    end

    @client.send('hello', 0)
    t.join

    data.first.should == 'hel'
  end

  it "returns an array with the data and connection info" do
    data = nil
    t = Thread.new do
      client = @server.accept
      data = client.recvfrom(3)
      client.close
    end

    @client.send('hello', 0)
    t.join

    data.size.should == 2
    data.first.should == "hel"
    # This does not apply to every platform, dependant on recvfrom(2)
    # data.last.should == nil
  end
  
end
