require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#abort" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:send)
    @socket.stub!(:readline).and_return("226 Closing data connection.")
    
    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end
  
  it "sends the ABOR command over the socket" do
    @socket.should_receive(:send).with("ABOR\r\n", Socket::MSG_OOB)
    @ftp.abort
  end
  
  it "returns the full response" do
    @socket.should_receive(:readline).and_return("226 Closing data connection.")
    @ftp.abort.should == "226 Closing data connection.\n"
  end
  
  it "does not raise any error when the response code is 226" do
    @socket.should_receive(:readline).and_return("225 Data connection open; no transfer in progress.")
    @ftp.abort
  end
  
  it "does not raise any error when the response code is 226" do
    @socket.should_receive(:readline).and_return("226 Closing data connection.")
    @ftp.abort
  end

  it "raises a Net::FTPProtoError when the response code is 500" do
    @socket.should_receive(:readline).and_return("500 Syntax error, command unrecognized.")
    lambda { @ftp.abort }.should raise_error(Net::FTPProtoError)
  end
  
  it "raises a Net::FTPProtoError when the response code is 501" do
    @socket.should_receive(:readline).and_return("501 Syntax error in parameters or arguments.")
    lambda { @ftp.abort }.should raise_error(Net::FTPProtoError)
  end

  it "raises a Net::FTPProtoError when the response code is 502" do
    @socket.should_receive(:readline).and_return("502 Command not implemented.")
    lambda { @ftp.abort }.should raise_error(Net::FTPProtoError)
  end

  it "raises a Net::FTPProtoError when the response code is 421" do
    @socket.should_receive(:readline).and_return("421 Service not available, closing control connection.")
    lambda { @ftp.abort }.should raise_error(Net::FTPProtoError)
  end
end