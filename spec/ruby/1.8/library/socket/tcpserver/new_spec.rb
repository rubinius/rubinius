require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "TCPServer.new" do
  after(:each) do
    @server.close if @server && !@server.closed?
  end
  
  it "binds to a host and a port" do
    @server = TCPServer.new('127.0.0.1', SocketSpecs.port)
    addr = @server.addr
    addr[0].should == 'AF_INET'
    addr[1].be_kind_of Fixnum
    # on some platforms (Mac), MRI
    # returns comma at the end.
    addr[2].should =~ /^localhost,?$/
    addr[3].should == '127.0.0.1'
  end
end
