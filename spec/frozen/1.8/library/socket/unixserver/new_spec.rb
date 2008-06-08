require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../fixtures/classes'
require 'tempfile'

describe "UNIXServer" do
  not_supported_on :windows do
    it "#new should create a new UNIXServer" do
      temp = Tempfile.new 'unixserver_spec'
      path = temp.path.dup
      temp.unlink
      unix = UNIXServer.new(path)
      unix.path.should == path
      unix.addr.should == ["AF_UNIX", path]
      lambda {unix.peeraddr}.should raise_error(Errno::ENOTCONN)
    end
  end
end
