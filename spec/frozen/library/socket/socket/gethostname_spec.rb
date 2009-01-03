require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'

describe "Socket.gethostname" do
  # This currently works in Unix and Windows. Feel free to add guards
  # for other platforms.
  it "returns the host name" do
    Socket.gethostname.should == `hostname`.strip
  end
end
