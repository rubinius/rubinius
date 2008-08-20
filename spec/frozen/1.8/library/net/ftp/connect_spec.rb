require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'
require "socket"

# TODO: Add specs for using the SOCKSSocket
describe "Net::FTP#connect" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:readline).and_return("220 Service ready for new user.")
    TCPSocket.stub!(:open).and_return(@socket)

    @ftp = Net::FTP.new
  end
  
  it "opens a new TCPSocket to the given host on the given port" do
    TCPSocket.should_receive(:open).with("some_host", 51).and_return(@socket)
    @ftp.connect("some_host", 51)
  end
  
  it "prints a small debug line when in debug mode" do
    @ftp.debug_mode = true
    lambda { @ftp.connect("some_host", 51) }.should output(/#{"connect: some_host, 51"}/)
  end

  it "does not raise any error when the response code is 220" do
    @socket.should_receive(:readline).and_return("220 Service ready for new user.")
    @ftp.connect("some_host", 51)
  end

  it "raises a Net::FTPReplyError when the response code is 120" do
    @socket.should_receive(:readline).and_return("120 Service ready in nnn minutes.")
    lambda { @ftp.connect("some_host", 51) }.should raise_error(Net::FTPReplyError)
  end

  it "raises a Net::FTPTempError when the response code is 421" do
    @socket.should_receive(:readline).and_return("421 Service not available, closing control connection.")
    lambda { @ftp.connect("some_host", 51) }.should raise_error(Net::FTPTempError)
  end
end