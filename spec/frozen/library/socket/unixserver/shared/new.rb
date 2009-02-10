require File.dirname(__FILE__) + '/../../../../spec_helper'
require File.dirname(__FILE__) + '/../../fixtures/classes'
require 'tempfile'

describe :unixserver_new, :shared => true do
  platform_is_not :windows do
    it "creates a new UNIXServer" do
      path = tmp("unixserver_spec")
      File.unlink(path) if File.exists?(path)
      unix = UNIXServer.new(path)
      unix.path.should == path
      unix.addr.should == ["AF_UNIX", path]
      File.unlink(path)
    end
  end
end
