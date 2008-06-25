require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Socket::BasicSocket#close_read" do
  before :each do
    @server = TCPServer.new(SocketSpecs.port)    
  end

  after :each do
    @server.close unless @server.closed?
  end

  compliant_on :ruby do
   
    it "closes the reading end of the socket" do
      @server.close_read
      lambda { @server.read }.should raise_error(IOError)
    end

    it "it works on sockets with closed ends" do
      @server.close_read
      lambda { @server.close_read }.should_not raise_error(Exception)
      lambda { @server.read }.should raise_error(IOError)
    end
  end

  not_compliant_on :ruby do

    it "closes the reading end of the socket" do
      @server.close_read
      lambda { @server.read }.should raise_error(IOError)
    end

    it "does not work on sockets with closed ends" do
      @server.close_read
      lambda { @server.close_read }.should raise_error(Exception)
    end
  end
end

