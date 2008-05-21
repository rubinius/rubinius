require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "TCPSocket#gethostbyname" do
  it "should resolve a hostname to an address" do
    a = TCPSocket.gethostbyname("localhost")
    # on some platforms (Mac), MRI
    # returns comma at the end.
    a.first.should =~ /^localhost,?$/
  end
end
