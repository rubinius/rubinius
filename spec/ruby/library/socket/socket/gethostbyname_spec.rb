require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

require 'socket'

describe "Socket#gethostbyname" do
  it "returns broadcast address info for '<broadcast>'" do
    addr = Socket.gethostbyname('<broadcast>').first;
    ["broadcasthost", "255.255.255.255"].should include(addr)
  end

  it "returns broadcast address info for '<any>'" do
    addr = Socket.gethostbyname('<any>').first;
    addr.should == "0.0.0.0"
  end
end
