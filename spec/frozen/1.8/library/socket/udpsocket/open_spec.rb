require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "UDPSocket.open" do
  after(:each) do
    @socket.close if @socket && !@socket.closed?
    @server.close if @server && !@server.closed?
  end

  it "allows calls to open without arguments" do
    @socket = UDPSocket.open
  end
end
