require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "TCPServer#gets" do
  before :each do
    @server = TCPServer.new(SocketSpecs.hostname, SocketSpecs.port)
  end

  after :each do
    @server.close
  end

  it "raises Errno::ENOTCONN on gets" do
    lambda { @server.gets }.should raise_error(Errno::ENOTCONN)
  end
end
