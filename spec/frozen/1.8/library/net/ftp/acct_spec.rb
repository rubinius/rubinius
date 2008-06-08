require File.dirname(__FILE__) + '/../../../spec_helper'
require 'net/ftp'

describe "Net::FTP#acct" do
  before(:each) do
    @socket = mock("Socket")
    @socket.stub!(:write)
    @socket.stub!(:readline).and_return("226 Success")

    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end

  it "writes the ACCT command to the socket" do
    @socket.should_receive(:write).with("ACCT my_account\r\n")
    @ftp.acct("my_account")
  end
  
  it "does not raise any error when the response code is 230" do
    @socket.should_receive(:readline).and_return("230 User logged in, proceed.")
    @ftp.acct("my_account")
  end
  
  it "raises a Net::FTPPermError when the response code is 530" do
    @socket.should_receive(:readline).and_return("530 Not logged in.")
    lambda { @ftp.acct("my_account") }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPPermError when the response code is 500" do
    @socket.should_receive(:readline).and_return("500 Syntax error, command unrecognized.")
    lambda { @ftp.acct("my_account") }.should raise_error(Net::FTPPermError)
  end
  
  it "raises a Net::FTPPermError when the response code is 501" do
    @socket.should_receive(:readline).and_return("501 Syntax error in parameters or arguments.")
    lambda { @ftp.acct("my_account") }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPPermError when the response code is 503" do
    @socket.should_receive(:readline).and_return("503 Bad sequence of commands.")
    lambda { @ftp.acct("my_account") }.should raise_error(Net::FTPPermError)
  end

  it "raises a Net::FTPTempError when the response code is 421" do
    @socket.should_receive(:readline).and_return("421 Service not available, closing control connection.")
    lambda { @ftp.acct("my_account") }.should raise_error(Net::FTPTempError)
  end
end
