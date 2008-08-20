require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#voidcmd" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:write)

    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end
  
  it "sends the passed command over the socket and returns nil" do
    @socket.should_receive(:readline).and_return("2xx Does not raise.")
    @ftp.voidcmd("CMD").should be_nil
  end
  
  it "raises a Net::FTPReplyError when the response code is 1xx" do
    @socket.should_receive(:readline).and_return("1xx Does raise a Net::FTPReplyError.")
    lambda { @ftp.voidcmd("CMD") }.should raise_error(Net::FTPReplyError)
  end

  it "raises a Net::FTPReplyError when the response code is 3xx" do
    @socket.should_receive(:readline).and_return("3xx Does raise a Net::FTPReplyError.")
    lambda { @ftp.voidcmd("CMD") }.should raise_error(Net::FTPReplyError)
  end

  it "raises a Net::FTPTempError when the response code is 4xx" do
    @socket.should_receive(:readline).and_return("4xx Does raise a Net::FTPTempError.")
    lambda { @ftp.voidcmd("CMD") }.should raise_error(Net::FTPTempError)
  end

  it "raises a Net::FTPPermError when the response code is 5xx" do
    @socket.should_receive(:readline).and_return("5xx Does raise a Net::FTPPermError.")
    lambda { @ftp.voidcmd("CMD") }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPProtoError when the response code is not valid" do
    @socket.should_receive(:readline).and_return("999 Does raise a Net::FTPProtoError.")
    lambda { @ftp.voidcmd("CMD") }.should raise_error(Net::FTPProtoError)
  end
end
