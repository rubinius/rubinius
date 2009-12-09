require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'
require File.dirname(__FILE__) + '/../shared/partially_closable_sockets'

describe "TCPSocket partial closability" do
  
  before :each do
    @server = TCPServer.new("127.0.0.1", SocketSpecs.port)
    @s1 = TCPSocket.new("127.0.0.1", SocketSpecs.port)
    @s2 = @server.accept
  end

  after :each do
    @server.close
    @s1.close
    @s2.close
  end
  
  it_should_behave_like "partially closable sockets"
  
end