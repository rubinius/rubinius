require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "UDPSocket.open" do
  after(:each) do
    @socket.close if @socket && !@socket.closed?
  end

  it "allows calls to open without arguments" do
    @socket = UDPSocket.open
    @socket.should be_kind_of(UDPSocket)
  end
end
